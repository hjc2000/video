//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsAbstractTable.h"
#include "tsBinaryTable.h"
#include "tsDuckContext.h"
#include "tsPSIRepository.h"
#include "tsSection.h"


//----------------------------------------------------------------------------
// Move constructor.
//----------------------------------------------------------------------------

ts::BinaryTable::BinaryTable(BinaryTable &&other) noexcept :
	_is_valid(other._is_valid),
	_tid(other._tid),
	_tid_ext(other._tid_ext),
	_version(other._version),
	_source_pid(other._source_pid),
	_missing_count(other._missing_count),
	_sections(std::move(other._sections))
{
	// If section array was actually moved, reset related data.
	if (other._sections.empty())
	{
		other._missing_count = 0;
	}
}


//----------------------------------------------------------------------------
// Copy constructor. The sections are either shared between the
// two tables or duplicated.
//----------------------------------------------------------------------------

ts::BinaryTable::BinaryTable(const BinaryTable &other, ShareMode mode) :
	_is_valid(other._is_valid),
	_tid(other._tid),
	_tid_ext(other._tid_ext),
	_version(other._version),
	_source_pid(other._source_pid),
	_missing_count(other._missing_count),
	_sections()
{
	switch (mode)
	{
	case ShareMode::SHARE:
		{
			// Copy the pointers, share the pointed sections
			_sections = other._sections;
			break;
		}
	case ShareMode::COPY:
		{
			_sections.resize(other._sections.size());
			for (size_t i = 0; i < _sections.size(); ++i)
			{
				if (other._sections[i].isNull())
				{
					_sections[i].clear();
				}
				else
				{
					_sections[i] = new Section(*other._sections[i], ShareMode::COPY);
				}
			}
			break;
		}
	default:
		{
			// should not get there
			assert(false);
		}
	}
}


//----------------------------------------------------------------------------
// Constructor from an array of sections.
//----------------------------------------------------------------------------

ts::BinaryTable::BinaryTable(const SectionPtrVector &sections, bool replace, bool grow)
{
	if (!addSections(sections, replace, grow))
	{
		clear();
	}
}


//----------------------------------------------------------------------------
// Assignment. The sections are referenced, and thus shared
// between the two table objects.
//----------------------------------------------------------------------------

ts::BinaryTable &ts::BinaryTable::operator=(const BinaryTable &other)
{
	if (&other != this)
	{
		_is_valid = other._is_valid;
		_tid = other._tid;
		_tid_ext = other._tid_ext;
		_version = other._version;
		_source_pid = other._source_pid;
		_missing_count = other._missing_count;
		_sections = other._sections;
	}
	return *this;
}


//----------------------------------------------------------------------------
// Move assignment.
//----------------------------------------------------------------------------

ts::BinaryTable &ts::BinaryTable::operator=(BinaryTable &&other) noexcept
{
	if (&other != this)
	{
		_is_valid = other._is_valid;
		_tid = other._tid;
		_tid_ext = other._tid_ext;
		_version = other._version;
		_source_pid = other._source_pid;
		_missing_count = other._missing_count;
		_sections = std::move(other._sections);
		// If section array was actually moved, reset related data.
		if (other._sections.empty())
		{
			other._missing_count = 0;
		}
	}
	return *this;
}


//----------------------------------------------------------------------------
// Duplication. Similar to assignment but the sections are duplicated.
//----------------------------------------------------------------------------

ts::BinaryTable &ts::BinaryTable::copy(const BinaryTable &table)
{
	_is_valid = table._is_valid;
	_tid = table._tid;
	_tid_ext = table._tid_ext;
	_version = table._version;
	_source_pid = table._source_pid;
	_missing_count = table._missing_count;
	_sections.resize(table._sections.size());
	for (size_t i = 0; i < _sections.size(); ++i)
	{
		if (table._sections[i].isNull())
		{
			_sections[i].clear();
		}
		else
		{
			_sections[i] = new Section(*table._sections[i], ShareMode::COPY);
		}
	}
	return *this;
}


//----------------------------------------------------------------------------
// Comparison. Note: Invalid tables are never identical
//----------------------------------------------------------------------------

bool ts::BinaryTable::operator==(const BinaryTable &table) const
{
	bool equal =
		_is_valid &&
		table._is_valid &&
		_tid == table._tid &&
		_tid_ext == table._tid_ext &&
		_version == table._version &&
		_sections.size() == table._sections.size();

	for (size_t i = 0; equal && i < _sections.size(); ++i)
	{
		equal = !_sections[i].isNull() && !table._sections[i].isNull() && *_sections[i] == *table._sections[i];
	}

	return equal;
}


//----------------------------------------------------------------------------
// Get a pointer to a section.
//----------------------------------------------------------------------------

const ts::SectionPtr ts::BinaryTable::sectionAt(size_t index) const
{
	return index < _sections.size() ? _sections[index] : SectionPtr();
}


//----------------------------------------------------------------------------
// Implementation of AbstractDefinedByStandards.
//----------------------------------------------------------------------------

ts::Standards ts::BinaryTable::definingStandards() const
{
	// The defining standard is taken from table id.
	return PSIRepository::Instance().getTableStandards(tableId(), _source_pid);
}


//----------------------------------------------------------------------------
// Modifiable properties.
//----------------------------------------------------------------------------

void ts::BinaryTable::setTableIdExtension(uint16_t tid_ext, bool recompute_crc)
{
	_tid_ext = tid_ext;
	for (const auto &it : _sections)
	{
		if (!it.isNull())
		{
			it->setTableIdExtension(tid_ext, recompute_crc);
		}
	}
}

void ts::BinaryTable::setVersion(uint8_t version, bool recompute_crc)
{
	_version = version;
	for (const auto &it : _sections)
	{
		if (!it.isNull())
		{
			it->setVersion(version, recompute_crc);
		}
	}
}

void ts::BinaryTable::setSourcePID(PID pid)
{
	_source_pid = pid;
	for (const auto &it : _sections)
	{
		if (!it.isNull())
		{
			it->setSourcePID(pid);
		}
	}
}


//----------------------------------------------------------------------------
// Index of first and last TS packet of the table in the demultiplexed stream.
//----------------------------------------------------------------------------

ts::PacketCounter ts::BinaryTable::firstTSPacketIndex() const
{
	bool found = false;
	PacketCounter first = std::numeric_limits<PacketCounter>::max();
	for (const auto &it : _sections)
	{
		if (!it.isNull())
		{
			found = true;
			first = std::min(first, it->firstTSPacketIndex());
		}
	}
	return found ? first : 0;
}

ts::PacketCounter ts::BinaryTable::lastTSPacketIndex() const
{
	PacketCounter last = 0;
	for (const auto &it : _sections)
	{
		if (!it.isNull())
		{
			last = std::max(last, it->lastTSPacketIndex());
		}
	}
	return last;
}


//----------------------------------------------------------------------------
// Clear the content of the table. The table must be rebuilt
// using calls to addSection.
//----------------------------------------------------------------------------

void ts::BinaryTable::clear()
{
	_is_valid = false;
	_tid = 0xFF;
	_tid_ext = 0;
	_version = 0;
	_source_pid = PID_NULL;
	_missing_count = 0;
	_sections.clear();
}


//----------------------------------------------------------------------------
// Return the total size in bytes of all sections in the table.
//----------------------------------------------------------------------------

size_t ts::BinaryTable::totalSize() const
{
	size_t size = 0;
	for (const auto &it : _sections)
	{
		if (!it.isNull() && it->isValid())
		{
			size += it->size();
		}
	}
	return size;
}


//----------------------------------------------------------------------------
// Minimum number of TS packets required to transport the table.
//----------------------------------------------------------------------------

ts::PacketCounter ts::BinaryTable::packetCount(bool pack) const
{
	return Section::PacketCount(_sections, pack);
}


//----------------------------------------------------------------------------
// Add several sections to a table
//----------------------------------------------------------------------------

bool ts::BinaryTable::addSections(SectionPtrVector::const_iterator first, SectionPtrVector::const_iterator last, bool replace, bool grow)
{
	bool ok = true;
	for (auto it = first; it != last; ++it)
	{
		ok = addSection(*it, replace, grow) && ok;
	}
	return ok;
}


//----------------------------------------------------------------------------
// A table is built by adding sections using addSection.
// When all sections are present, the table becomes valid.
//----------------------------------------------------------------------------

bool ts::BinaryTable::addSection(const SectionPtr &sect, bool replace, bool grow)
{
	// Reject invalid sections

	if (sect.isNull() || !sect->isValid())
	{
		return false;
	}

	// Check the compatibility of the section with the table

	const int index = sect->sectionNumber();

	if (_sections.size() == 0)
	{
		// This is the first section, set the various parameters
		_sections.resize(size_t(sect->lastSectionNumber()) + 1);
		assert(index < int(_sections.size()));
		_tid = sect->tableId();
		_tid_ext = sect->tableIdExtension();
		_version = sect->version();
		_source_pid = sect->sourcePID();
		_missing_count = int(_sections.size());
	}
	else if (sect->tableId() != _tid || sect->tableIdExtension() != _tid_ext || sect->version() != _version)
	{
		// Not the same table
		return false;
	}
	else if (!grow && (index >= int(_sections.size()) || size_t(sect->lastSectionNumber()) != _sections.size() - 1))
	{
		// Incompatible number of sections
		return false;
	}
	else if (size_t(sect->lastSectionNumber()) != _sections.size() - 1)
	{
		// Incompatible number of sections but the table is allowed to grow
		if (size_t(sect->lastSectionNumber()) < _sections.size() - 1)
		{
			// The new section must be updated
			sect->setLastSectionNumber(uint8_t(int(_sections.size()) - 1));
		}
		else
		{
			// The table must be updated (more sections)
			_missing_count += int(sect->lastSectionNumber()) + 1 - int(_sections.size());
			_sections.resize(size_t(sect->lastSectionNumber()) + 1);
			assert(index < int(_sections.size()));
			// Modify all previously entered sections
			for (int si = 0; si < int(_sections.size()); ++si)
			{
				if (!_sections[si].isNull())
				{
					_sections[si]->setLastSectionNumber(sect->lastSectionNumber());
				}
			}
		}
	}

	// Now add the section

	if (_sections[index].isNull())
	{
		// The section was not present, add it
		_sections[index] = sect;
		_missing_count--;
	}
	else if (!replace)
	{
		// Section already present, don't replace
		return false;
	}
	else
	{
		// Section already present but replace
		_sections[index] = sect;
	}

	// The table becomes valid if there is no more missing section
	_is_valid = _missing_count == 0;
	assert(_missing_count >= 0);

	return true;
}


//----------------------------------------------------------------------------
// Pack all sections in a table, removing references to missing sections.
//----------------------------------------------------------------------------

bool ts::BinaryTable::packSections()
{
	// There is nothing to do if no section is missing.
	if (_missing_count > 0)
	{
		assert(!_is_valid);
		assert(!_sections.empty());

		// Next section number to copy.
		size_t next_section = 0;

		// Pack all section pointers.
		for (size_t n = 0; n < _sections.size(); ++n)
		{
			if (!_sections[n].isNull())
			{
				if (next_section != n)
				{
					_sections[next_section] = _sections[n];
				}
				++next_section;
			}
		}

		// Resize to new number of sections.
		_sections.resize(next_section);
		_missing_count = 0;
		_is_valid = !_sections.empty();

		// Now patch section numbers.
		for (size_t n = 0; n < _sections.size(); ++n)
		{
			assert(!_sections[n].isNull());
			assert(next_section > 0);
			_sections[n]->setSectionNumber(uint8_t(n), false);
			_sections[n]->setLastSectionNumber(uint8_t(next_section - 1), true);
		}
	}
	return _is_valid;
}


//----------------------------------------------------------------------------
// Check if this is a table with one short section.
//----------------------------------------------------------------------------

bool ts::BinaryTable::isShortSection() const
{
	return _sections.size() == 1 && !_sections[0].isNull() && _sections[0]->isShortSection();
}


//----------------------------------------------------------------------------
// Options to convert a binary table into XML.
//----------------------------------------------------------------------------

ts::BinaryTable::XMLOptions::XMLOptions() :
	forceGeneric(false),
	setPID(false),
	setLocalTime(false),
	setPackets(false)
{
}
