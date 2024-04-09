//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsARIBCharset.h"
#include "tsCerrReport.h"
#include "tsDVBCharTableSingleByte.h"
#include "tsDVBCharTableUTF8.h"
#include "tsDuckConfigFile.h"
#include "tsDuckContext.h"
#include "tsTime.h"


//----------------------------------------------------------------------------
// Constructor and destructors.
//----------------------------------------------------------------------------

ts::DuckContext::DuckContext(Report *report, std::ostream *output) :
	_report(report != nullptr ? report : &CERR),
	_initial_out(output != nullptr ? output : &std::cout),
	_out(_initial_out),
	_charsetIn(&DVBCharTableSingleByte::DVB_ISO_6937),  // default DVB charset
	_charsetOut(&DVBCharTableSingleByte::DVB_ISO_6937),
	_timeRefConfig(DuckConfigFile::Instance().value(u"default.time")),
	_predefined_cas{ {CASID_CONAX_MIN,      u"conax"},
					{CASID_IRDETO_MIN,     u"irdeto"},
					{CASID_MEDIAGUARD_MIN, u"mediaguard"},
					{CASID_NAGRA_MIN,      u"nagravision"},
					{CASID_NDS_MIN,        u"nds"},
					{CASID_SAFEACCESS,     u"safeaccess"},
					{CASID_VIACCESS_MIN,   u"viaccess"},
					{CASID_WIDEVINE_MIN,   u"widevine"} }
{
	// Initialize time reference from configuration file. Ignore errors.
	if (!_timeRefConfig.empty() && !setTimeReference(_timeRefConfig))
	{
		CERR.verbose(u"invalid default.time '%s' in %s", { _timeRefConfig, DuckConfigFile::Instance().fileName() });
	}

	// Get leap.seconds initial value from configuration file. Default value is true.
	const UString ls(DuckConfigFile::Instance().value(u"leap.seconds"));
	if (!ls.empty() && !ls.toBool(_useLeapSeconds))
	{
		_useLeapSeconds = true;
		CERR.verbose(u"invalid leap.seconds '%s' in %s", { ls, DuckConfigFile::Instance().fileName() });
	}
}


//----------------------------------------------------------------------------
// Reset the TSDuck context to initial configuration.
//----------------------------------------------------------------------------

void ts::DuckContext::reset()
{
	if (_outFile.is_open())
	{
		_outFile.close();
	}

	_out = _initial_out;
	_charsetIn = _charsetOut = &DVBCharTableSingleByte::DVB_ISO_6937;
	_casId = CASID_NULL;
	_defaultPDS = 0;
	_cmdStandards = _accStandards = Standards::NONE;
	_hfDefaultRegion.clear();
	_timeReference = 0;
}


//----------------------------------------------------------------------------
// Set a new report for log and error messages.
//----------------------------------------------------------------------------

void ts::DuckContext::setReport(Report *report)
{
	_report = report != nullptr ? report : &CERR;
}


//----------------------------------------------------------------------------
// Set the DVB character sets (default DVB character set if null).
//----------------------------------------------------------------------------

void ts::DuckContext::setDefaultCharsetIn(const Charset *charset)
{
	_charsetIn = charset != nullptr ? charset : &DVBCharTableSingleByte::DVB_ISO_6937;
}

void ts::DuckContext::setDefaultCharsetOut(const Charset *charset)
{
	_charsetOut = charset != nullptr ? charset : &DVBCharTableSingleByte::DVB_ISO_6937;
}


//----------------------------------------------------------------------------
// Update the list of standards which are present in the context.
//----------------------------------------------------------------------------

void ts::DuckContext::addStandards(Standards mask)
{
	if (_report->debug() && (_accStandards | mask) != _accStandards)
	{
		_report->debug(u"adding standards %s to %s", { StandardsNames(mask), StandardsNames(_accStandards) });
	}
	_accStandards |= mask;
}

void ts::DuckContext::resetStandards(Standards mask)
{
	_accStandards = _cmdStandards | mask;

	if (_report->debug())
	{
		_report->debug(u"resetting standards to %s", { StandardsNames(_accStandards) });
	}
}


//----------------------------------------------------------------------------
// The actual CAS id to use.
//----------------------------------------------------------------------------

void ts::DuckContext::setDefaultCASId(uint16_t cas)
{
	_casId = cas;
}

uint16_t ts::DuckContext::casId(uint16_t cas) const
{
	return cas == CASID_NULL ? _casId : cas;
}


//----------------------------------------------------------------------------
// The actual private data specifier to use.
//----------------------------------------------------------------------------

void ts::DuckContext::setDefaultPDS(PDS pds)
{
	_defaultPDS = pds;
}

ts::PDS ts::DuckContext::actualPDS(PDS pds) const
{
	if (pds != 0)
	{
		// Explicit PDS already defined.
		return pds;
	}
	else if (_defaultPDS != 0)
	{
		// A default PDS was specified.
		return _defaultPDS;
	}
	else if (bool(_accStandards & Standards::ATSC))
	{
		// We have previously found ATSC signalization, use the fake PDS for ATSC.
		// This allows interpretation of ATSC descriptors in MPEG-defined tables (eg. PMT).
		return PDS_ATSC;
	}
	else if (bool(_accStandards & Standards::ISDB))
	{
		// Same principle for ISDB.
		return PDS_ISDB;
	}
	else
	{
		// Really no PDS to use.
		return 0;
	}
}


//----------------------------------------------------------------------------
// Registration ids (from MPEG-defined registration_descriptor).
//----------------------------------------------------------------------------

void ts::DuckContext::addRegistrationId(uint32_t regid)
{
	if (regid != REGID_NULL)
	{
		_lastRegistrationId = regid;
		_registrationIds.insert(regid);
	}
}

void ts::DuckContext::resetRegistrationIds()
{
	_lastRegistrationId = REGID_NULL;
	_registrationIds.clear();
}


//----------------------------------------------------------------------------
// Name of the default region for UVH and VHF band frequency layout.
//----------------------------------------------------------------------------

void ts::DuckContext::setDefaultHFRegion(const UString &region)
{
	_hfDefaultRegion = region;
}

ts::UString ts::DuckContext::defaultHFRegion() const
{
	// If the region is empty, get the one for the TSDuck configuration file.
	if (!_hfDefaultRegion.empty())
	{
		return _hfDefaultRegion;
	}
	else
	{
		return DuckConfigFile::Instance().value(u"default.region", u"europe");
	}
}

//----------------------------------------------------------------------------
// Set a non-standard time reference offset using a name.
//----------------------------------------------------------------------------

bool ts::DuckContext::setTimeReference(const UString &name)
{
	// Convert to uppercase without space.
	UString str(name);
	str.convertToUpper();
	str.remove(SPACE);

	if (str == u"UTC")
	{
		_timeReference = 0;
		return true;
	}
	else if (str == u"JST")
	{
		_timeReference = Time::JSTOffset;
		return true;
	}

	size_t count = 0;
	size_t last = 0;
	UChar sign = CHAR_NULL;
	SubSecond hours = 0;
	SubSecond minutes = 0;

	str.scan(count, last, u"UTC%c%d:%d", { &sign, &hours, &minutes });
	if ((count == 2 || count == 3) &&
		last == str.size() &&
		(sign == u'+' || sign == u'-') &&
		hours >= 0 && hours <= 12 &&
		minutes >= 0 && minutes <= 59)
	{
		_timeReference = (sign == u'+' ? +1 : -1) * (hours * MilliSecPerHour + minutes * MilliSecPerMin);
		return true;
	}
	else
	{
		// Incorrect name.
		return false;
	}
}


//----------------------------------------------------------------------------
// Get the non-standard time reference offset as a string.
//----------------------------------------------------------------------------

ts::UString ts::DuckContext::timeReferenceName() const
{
	if (_timeReference == 0)
	{
		return u"UTC";  // no offset
	}
	else if (_timeReference == Time::JSTOffset)
	{
		return u"JST";
	}
	else
	{
		const UChar sign = _timeReference < 0 ? u'-' : u'+';
		const SubSecond hours = std::abs(_timeReference) / MilliSecPerHour;
		const SubSecond minutes = (std::abs(_timeReference) / MilliSecPerMin) % 60;
		if (minutes == 0)
		{
			return UString::Format(u"UTC%c%d", { sign, hours });
		}
		else
		{
			return UString::Format(u"UTC%c%d:%02d", { sign, hours, minutes });
		}
	}
}


//----------------------------------------------------------------------------
// Flush the text output.
//----------------------------------------------------------------------------

void ts::DuckContext::flush()
{
	// Flush the output.
	_out->flush();

	// On Unix, we must force the lower-level standard output.
	#if defined(TS_UNIX)
	if (_out == &std::cout)
	{
		::fflush(stdout);
		::fsync(STDOUT_FILENO);
	}
	else if (_out == &std::cerr)
	{
		::fflush(stderr);
		::fsync(STDERR_FILENO);
	}
	#endif
}


//----------------------------------------------------------------------------
// Redirect the output stream to a file.
//----------------------------------------------------------------------------

void ts::DuckContext::setOutput(std::ostream *stream, bool override)
{
	// Do not override output if not standard output (or explicit override).
	if (override || _out == &std::cout)
	{
		if (_out == &_outFile)
		{
			_outFile.close();
		}
		_out = stream == nullptr ? &std::cout : stream;
	}
}

bool ts::DuckContext::setOutput(const fs::path &fileName, bool override)
{
	// Do not override output if not standard output (or explicit override).
	if (override || _out == &std::cout)
	{
		// Close previous file, if any.
		if (_out == &_outFile)
		{
			_outFile.close();
			_out = &std::cout;
		}

		// Open new file if any.
		if (!fileName.empty() && fileName != u"-")
		{
			_report->verbose(u"creating %s", { fileName });
			_outFile.open(fileName, std::ios::out);
			if (!_outFile)
			{
				_report->error(u"cannot create %s", { fileName });
				return false;
			}
			_out = &_outFile;
		}
	}
	return true;
}

//----------------------------------------------------------------------------
// An opaque class to save all command line options, as loaded by loadArgs().
//----------------------------------------------------------------------------

ts::DuckContext::SavedArgs::SavedArgs() :
	_definedCmdOptions(0),
	_cmdStandards(Standards::NONE),
	_charsetInName(),
	_charsetOutName(),
	_casId(CASID_NULL),
	_defaultPDS(0),
	_hfDefaultRegion(),
	_timeReference(0)
{
}

void ts::DuckContext::saveArgs(SavedArgs &args) const
{
	args._definedCmdOptions = _definedCmdOptions;
	args._cmdStandards = _cmdStandards;
	args._charsetInName = _charsetIn->name();
	args._charsetOutName = _charsetOut->name();
	args._casId = _casId;
	args._defaultPDS = _defaultPDS;
	args._hfDefaultRegion = _hfDefaultRegion;
	args._timeReference = _timeReference;
}

void ts::DuckContext::restoreArgs(const SavedArgs &args)
{
	if (args._definedCmdOptions & CMD_STANDARDS)
	{
		// Reset accumulated standards if a list of standards was saved.
		_accStandards = _cmdStandards = args._cmdStandards;
	}
	if (args._definedCmdOptions & CMD_CHARSET)
	{
		const Charset *in = DVBCharTable::GetCharset(args._charsetInName);
		const Charset *out = DVBCharTable::GetCharset(args._charsetOutName);
		if (in != nullptr)
		{
			_charsetIn = in;
		}
		if (out != nullptr)
		{
			_charsetOut = out;
		}
	}
	if (_definedCmdOptions & CMD_CAS)
	{
		_casId = args._casId;
	}
	if (_definedCmdOptions & CMD_PDS)
	{
		_defaultPDS = args._defaultPDS;
	}
	if (_definedCmdOptions & CMD_HF_REGION)
	{
		_hfDefaultRegion = args._hfDefaultRegion;
	}
	if (_definedCmdOptions & CMD_TIMEREF)
	{
		_timeReference = args._timeReference;
	}
}
