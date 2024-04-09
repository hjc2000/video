//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsAlgorithm.h"
#include "tsDuckContext.h"
#include "tsPAT.h"
#include "tsSection.h"
#include "tsTablesLoggerFilter.h"
#include "tsTablesLoggerFilterRepository.h"

// Register this section filter in the reposity.
namespace
{
	ts::TablesLoggerFilterInterface *_Factory17()
	{
		return new ts::TablesLoggerFilter;
	}
} static ts::TablesLoggerFilterRepository::Register _Registrar17(_Factory17);

//----------------------------------------------------------------------------
// Reset context, if filtering restarts from the beginning for instance.
//----------------------------------------------------------------------------

bool ts::TablesLoggerFilter::reset()
{
	_current_pids = _pids;
	_pat.clear();
	return true;
}


//----------------------------------------------------------------------------
// Check if a specific section must be filtered and displayed.
//----------------------------------------------------------------------------

bool ts::TablesLoggerFilter::filterSection(DuckContext &duck, const Section &section, uint16_t cas, PIDSet &more_pids)
{
	// Accumulate PAT data when --psi-si is specified to detect PMT PID's.
	if (_psi_si && section.tableId() == TID_PAT)
	{
		// Previous state of the PAT.
		const bool was_valid = _pat.isValid();
		const uint8_t previous_version = _pat.version();
		// Clear previous PAT on new version.
		if (_pat.sectionCount() > 0 && previous_version != section.version())
		{
			_pat.clear();
		}
		// Add the current section in the PAT if it was not already there.
		if (_pat.sectionCount() <= section.sectionNumber() || _pat.sectionAt(section.sectionNumber()).isNull())
		{
			_pat.addSection(SectionPtr(new Section(section, ShareMode::SHARE)), true, true);
		}
		// If a new PAT is now available, analyze it to grab PSI/SI information.
		if (_pat.isValid() && (!was_valid || _pat.version() != previous_version))
		{
			const PAT new_pat(duck, _pat);
			if (new_pat.isValid())
			{
				// Check NIT PID, if present.
				if (new_pat.nit_pid != PID_NULL && !_current_pids.test(new_pat.nit_pid))
				{
					// The NIT PID was not yet known.
					_current_pids.set(new_pat.nit_pid);
					more_pids.set(new_pat.nit_pid);
				}
				// Check all PMT PID's.
				for (const auto &it : new_pat.pmts)
				{
					const PID pmt_pid = it.second;
					if (pmt_pid != PID_NULL && !_current_pids.test(pmt_pid))
					{
						// This PMT PID was not yet known.
						_current_pids.set(pmt_pid);
						more_pids.set(pmt_pid);
					}
				}
			}
		}
	}

	// Is this a selected TID or TID-ext?
	const bool tid_set = Contains(_tids, section.tableId());
	const bool tidext_set = Contains(_tidexts, section.tableIdExtension());
	const bool secnum_set = Contains(_secnums, section.sectionNumber());

	// Return final verdict. For each criteria (--pid, --tid, etc), either the criteria is
	// not specified or the corresponding value matches.
	return
		// Check PID:
		(_current_pids.none() || _current_pids.test(section.sourcePID())) &&
		// Check TID:
		(_tids.empty() || (tid_set && !_negate_tid) || (!tid_set && _negate_tid)) &&
		// Check TIDext:
		(!section.isLongSection() || _tidexts.empty() || (tidext_set && !_negate_tidext) || (!tidext_set && _negate_tidext)) &&
		// Check section number:
		(_secnums.empty() || (secnum_set && !_negate_secnum) || (!secnum_set && _negate_secnum)) &&
		// Check section content:
		(_content_filter.empty() || section.matchContent(_content_filter, _content_mask)) &&
		// Diversified payload ok
		(!_diversified || section.hasDiversifiedPayload());
}
