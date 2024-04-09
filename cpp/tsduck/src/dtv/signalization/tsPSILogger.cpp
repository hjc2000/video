//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsBinaryTable.h"
#include "tsDuckContext.h"
#include "tsPSILogger.h"
#include "tsSectionFile.h"
#include "tsTSPacket.h"
#include "tsTablesDisplay.h"
#include "tsjsonArray.h"
#include "tsjsonObject.h"
#include "tsxmlElement.h"

#define MIN_CLEAR_PACKETS 100000


//----------------------------------------------------------------------------
// Constructors and destructors
//----------------------------------------------------------------------------

ts::PSILogger::PSILogger(TablesDisplay &display) :
	_display(display),
	_duck(_display.duck()),
	_report(_duck.report())
{
}

ts::PSILogger::~PSILogger()
{
	close();
}

//----------------------------------------------------------------------------
// Open / close the PSI logger.
//----------------------------------------------------------------------------

bool ts::PSILogger::open()
{
	// Reset content.
	_xml_doc.clear();
	_x2j_conv.clear();
	_clear_packets_cnt = _scrambled_packets_cnt = 0;
	_previous_pat.clear();
	_previous_pat.invalidate();

	// Set XML options in document.
	_xml_doc.setTweaks(_xml_tweaks);
	_x2j_conv.setTweaks(_xml_tweaks);

	// Load the XML model for tables if we need to convert to JSON.
	if ((_use_json || _log_json_line) && !SectionFile::LoadModel(_x2j_conv))
	{
		return false;
	}

	// Open/create the destination
	if (_use_text)
	{
		if (!_duck.setOutput(_text_destination))
		{
			_abort = true;
			return false;
		}
		// Initial blank line
		_duck.out() << std::endl;
	}

	// Open/create the XML output.
	if (_use_xml && !_xml_doc.open(u"tsduck", u"", _xml_destination, std::cout))
	{
		_abort = true;
		return false;
	}

	// Open/create the JSON output.
	if (_use_json)
	{
		json::ValuePtr root;
		if (_xml_tweaks.x2jIncludeRoot)
		{
			root = new json::Object;
			root->add(u"#name", u"tsduck");
			root->add(u"#nodes", json::ValuePtr(new json::Array));
		}
		if (!_json_doc.open(root, _json_destination, std::cout))
		{
			_abort = true;
			return false;
		}
	}

	// Specify the PID filters
	_demux.reset();
	if (!_cat_only)
	{
		_demux.addPID(PID_PAT);   // MPEG
		_demux.addPID(PID_TSDT);  // MPEG
		_demux.addPID(PID_SDT);   // DVB, ISDB (also contain BAT)
		_demux.addPID(PID_PCAT);  // ISDB
		_demux.addPID(PID_BIT);   // ISDB
		_demux.addPID(PID_LDT);   // ISDB (also contain NBIT)
		_demux.addPID(PID_PSIP);  // ATSC
	}
	if (!_clear)
	{
		_demux.addPID(PID_CAT);
	}

	// Type of sections to get.
	_demux.setCurrentNext(_use_current, _use_next);

	return true;
}

void ts::PSILogger::close()
{
	_xml_doc.close();
	_json_doc.close();
}


//----------------------------------------------------------------------------
// Return true when the analysis is complete.
//----------------------------------------------------------------------------

bool ts::PSILogger::completed() const
{
	return _abort || (!_all_versions && _pat_ok && _cat_ok && _sdt_ok && _received_pmt >= _expected_pmt);
}


//----------------------------------------------------------------------------
// The following method feeds the logger with a TS packet.
//----------------------------------------------------------------------------

void ts::PSILogger::feedPacket(const TSPacket &pkt)
{
	// Feed the packet to the demux
	_demux.feedPacket(pkt);

	// On clear streams, there is no CAT (usually). To avoid waiting indefinitely,
	// if no CAT and no scrambled packet is found after a defined number of packets
	// (~ 4 seconds at satellite bitrate), no longer expect a CAT.
	if (pkt.getScrambling() == SC_CLEAR)
	{
		_clear_packets_cnt++;
	}
	else
	{
		_scrambled_packets_cnt++;
	}
	if (_scrambled_packets_cnt == 0 && _clear_packets_cnt > MIN_CLEAR_PACKETS)
	{
		_cat_ok = true;
	}

	// Check if the list of standards has changed.
	const Standards new_standards = _duck.standards();
	if (new_standards != _standards)
	{
		_report.debug(u"standards are now %s", { StandardsNames(new_standards) });
		_standards = new_standards;
	}
}


//----------------------------------------------------------------------------
// This hook is invoked when a complete table is available.
//----------------------------------------------------------------------------

void ts::PSILogger::handleTable(SectionDemux &, const BinaryTable &table)
{
	assert(table.sectionCount() > 0);

	const TID tid = table.tableId();
	const PID pid = table.sourcePID();

	switch (table.tableId())
	{

	case TID_PAT:
		{
			PAT pat(_duck, table);
			if (pid != PID_PAT)
			{
				// A PAT is only expected on PID 0
				_report.warning(u"got unexpected PAT on PID %d (0x%<X)", { pid });
			}
			else if (pat.isValid())
			{
				// Got the PAT.
				_pat_ok = true;
				// Stop filtering the PAT PID if we don't need all versions.
				if (!_all_versions)
				{
					_demux.removePID(pid);
				}
				// Reset all PMT PID's which disappeared or changed.
				if (_previous_pat.isValid())
				{
					for (const auto &prev_it : _previous_pat.pmts)
					{
						const auto new_it = pat.pmts.find(prev_it.first);
						if (new_it == pat.pmts.end() || new_it->second != prev_it.second)
						{
							// Service disappeared or changed PMT PID, remove the previous PMT PID.
							_demux.removePID(prev_it.second);
						}
					}
				}
				_previous_pat = pat;
				// Add a filter on each referenced PID to get the PMT
				for (const auto &it : pat.pmts)
				{
					_demux.addPID(it.second);
					_expected_pmt++;
				}
				// Also include NIT (considered as a PMT)
				_demux.addPID(pat.nit_pid != PID_NULL ? pat.nit_pid : PID(PID_NIT));
				_expected_pmt++;
			}
			displayTable(table);
			break;
		}

	case TID_CAT:
		{
			if (pid != PID_CAT)
			{
				// A CAT is only expected on PID 1
				_report.warning(u"got unexpected CAT on PID %d (0x%<X)", { pid });
			}
			else
			{
				// Got the CAT.
				_cat_ok = true;
				// Stop filtering the CAT PID if we don't need all versions.
				if (!_all_versions)
				{
					_demux.removePID(pid);
				}
			}
			displayTable(table);
			break;
		}

	case TID_NIT_ACT:  // NIT and PMT are processed identically.
	case TID_PMT:
		{
			// Stop filtering this PID if we don't need all versions.
			if (!_all_versions)
			{
				_demux.removePID(pid);
				_received_pmt++;
			}
			displayTable(table);
			break;
		}

	case TID_NIT_OTH:
		{
			// Ignore NIT for other networks if only one version required
			if (_all_versions)
			{
				displayTable(table);
			}
			break;
		}

	case TID_TSDT:
		{
			if (pid != PID_TSDT)
			{
				// A TSDT is only expected on PID 0x0002
				_report.warning(u"got unexpected TSDT on PID %d (0x%<X)", { pid });
			}
			else if (!_all_versions)
			{
				_demux.removePID(pid);
			}
			displayTable(table);
			break;
		}

	case TID_SDT_ACT:
		{
			if (pid != PID_SDT)
			{
				// An SDT is only expected on PID 0x0011
				_report.warning(u"got unexpected SDT on PID %d (0x%<X)", { pid });
				displayTable(table);
			}
			else if (_all_versions || !_sdt_ok)
			{
				_sdt_ok = true;
				// We cannot stop filtering this PID if we don't need all versions since a BAT can also be found here.
				displayTable(table);
			}
			break;
		}

	case TID_SDT_OTH:
		{
			// Ignore SDT for other networks if only one version required
			if (_all_versions)
			{
				displayTable(table);
			}
			break;
		}

	case TID_BAT:
		{
			if (pid != PID_BAT)
			{
				// An SDT is only expected on PID 0x0011
				_report.warning(u"got unexpected BAT on PID %d (0x%<X)", { pid });
				displayTable(table);
			}
			else if (_all_versions || !_bat_ok)
			{
				// Got the BAT.
				_bat_ok = true;
				// We cannot stop filtering this PID if we don't need all versions since the SDT can also be found here.
				displayTable(table);
			}
			break;
		}

	case TID_PCAT:
		{
			if (pid != PID_PCAT)
			{
				// An ISDB PCAT is only expected on PID 0x0022
				_report.warning(u"got unexpected ISDB PCAT on PID %d (0x%<X)", { pid });
			}
			else if (!_all_versions)
			{
				_demux.removePID(pid);
			}
			displayTable(table);
			break;
		}

	case TID_BIT:
		{
			if (pid != PID_BIT)
			{
				// An ISDB BIT is only expected on PID 0x0024
				_report.warning(u"got unexpected ISDB BIT on PID %d (0x%<X)", { pid });
			}
			else if (!_all_versions)
			{
				_demux.removePID(pid);
			}
			displayTable(table);
			break;
		}

	case TID_NBIT_REF:
	case TID_NBIT_BODY:
		{
			if (pid != PID_NBIT)
			{
				// An ISDB NBIT is only expected on PID 0x0025
				_report.warning(u"got unexpected ISDB NBIT on PID %d (0x%<X)", { pid });
			}
			// We cannot stop filtering this PID if we don't need all versions since the LDT can also be found here.
			displayTable(table);
			break;
		}

		// case TID_LDT: (same value as TID_MGT)
	case TID_MGT:
		{
			// ATSC MGT and ISDB LDT use the same table id, so it can be any.
			if (pid != PID_PSIP && pid != PID_LDT)
			{
				// An ATSC MGT is only expected on PID 0x1FFB.
				// An ISDB LDT is only expected on PID 0x0025.
				_report.warning(u"got unexpected ATSC MGT / ISDB LDT on PID %d (0x%<X)", { pid });
			}
			// We cannot stop filtering this PID if we don't need all versions
			// since the TVCT or CVCT (ATSC) and NBIT (ISDB) can also be found here.
			displayTable(table);
			break;
		}

	case TID_TVCT:
	case TID_CVCT:
		{
			// ATSC tables with channel description.
			displayTable(table);
			break;
		}

	default:
		{
			if (_duck.report().verbose())
			{
				_report.warning(u"got unexpected TID %d (0x%<X) on PID %d (0x%<X)", { tid, pid });
			}
		}
	}
}


//----------------------------------------------------------------------------
// This hook is invoked when a complete section is available.
// Only used with option --dump.
//----------------------------------------------------------------------------

void ts::PSILogger::handleSection(SectionDemux &, const Section &sect)
{
	sect.dump(_duck.out()) << std::endl;
}


//----------------------------------------------------------------------------
// Displays a binary table.
//----------------------------------------------------------------------------

void ts::PSILogger::displayTable(const BinaryTable &table)
{
	// Text output.
	if (_use_text)
	{
		_display.displayTable(table);
		_duck.out() << std::endl;
	}

	// XML options.
	BinaryTable::XMLOptions xml_options;
	xml_options.setPID = true;

	// Full XML output.
	if (_use_xml)
	{
		// Convert the table into an XML structure.
		table.toXML(_duck, _xml_doc.rootElement(), xml_options);

		// Print and delete the new table.
		_xml_doc.flush();
	}

	// Save table in JSON format.
	if (_use_json)
	{
		// First, build an XML document with the table.
		xml::Document doc(_report);
		doc.initialize(u"tsduck");
		table.toXML(_duck, doc.rootElement(), xml_options);

		// Convert to JSON. Force "tsduck" root to appear so that the path to the first table is always the same.
		// Query the first (and only) converted table and add it to the running document.
		_json_doc.add(_x2j_conv.convertToJSON(doc, true)->query(u"#nodes[0]"));
	}

	// XML and/or JSON one-liner in the log.
	if (_log_xml_line || _log_json_line)
	{

		// Build an XML document.
		xml::Document doc;
		doc.initialize(u"tsduck");

		// Convert the table into an XML structure.
		xml::Element *elem = table.toXML(_duck, doc.rootElement(), xml_options);
		if (elem != nullptr)
		{
			// Log the XML line.
			if (_log_xml_line)
			{
				_report.info(_log_xml_prefix + doc.oneLiner());
			}

			// Log the JSON line.
			if (_log_json_line)
			{
				// Convert the XML document into JSON.
				// Force "tsduck" root to appear so that the path to the first table is always the same.
				const json::ValuePtr root(_x2j_conv.convertToJSON(doc, true));

				// Query the first (and only) converted table and log it as one line.
				_report.info(_log_json_prefix + root->query(u"#nodes[0]").oneLiner(_report));
			}
		}
	}

	// Notify table, either at once or section by section.
	if (_table_handler != nullptr)
	{
		_table_handler->handleTable(_demux, table);
	}
	else if (_section_handler != nullptr)
	{
		for (size_t i = 0; i < table.sectionCount(); ++i)
		{
			_section_handler->handleSection(_demux, *table.sectionAt(i));
		}
	}
}


//----------------------------------------------------------------------------
// Report the demux errors (if any)
//----------------------------------------------------------------------------

void ts::PSILogger::reportDemuxErrors()
{
	if (_demux.hasErrors())
	{
		SectionDemux::Status status(_demux);
		_duck.out() << "* PSI/SI analysis errors:" << std::endl;
		status.display(_duck.out(), 4, true);
	}
}
