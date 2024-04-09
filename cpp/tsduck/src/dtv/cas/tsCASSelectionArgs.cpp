//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsCASSelectionArgs.h"
#include "tsPIDOperator.h"

const std::vector<ts::CASSelectionArgs::PredefinedCAS> ts::CASSelectionArgs::_predefined_cas{
	{u"conax",       CASID_CONAX_MIN,      CASID_CONAX_MAX},
	{u"irdeto",      CASID_IRDETO_MIN,     CASID_IRDETO_MAX},
	{u"mediaguard",  CASID_MEDIAGUARD_MIN, CASID_MEDIAGUARD_MAX},
	{u"nagravision", CASID_NAGRA_MIN,      CASID_NAGRA_MAX},
	{u"nds",         CASID_NDS_MIN,        CASID_NDS_MAX},
	{u"safeaccess",  CASID_SAFEACCESS,     CASID_SAFEACCESS},
	{u"viaccess",    CASID_VIACCESS_MIN,   CASID_VIACCESS_MAX},
	{u"widevine",    CASID_WIDEVINE_MIN,   CASID_WIDEVINE_MAX}
};

//----------------------------------------------------------------------------
// Check if the specified CAS or operator id matches the selection criteria.
//----------------------------------------------------------------------------

bool ts::CASSelectionArgs::casMatch(uint16_t cas) const
{
	// If min and max CAS ids are zero, this means all CAS.
	return (min_cas_id == 0 && max_cas_id == 0) || (cas >= min_cas_id && cas <= max_cas_id);
}

bool ts::CASSelectionArgs::operatorMatch(uint32_t oper) const
{
	// If cas_oper is zero, this means all operators.
	return cas_oper == 0 || oper == cas_oper;
}


//----------------------------------------------------------------------------
// Analyze all CA_descriptors in a CAT or PMT and locate all matching PID's.
//----------------------------------------------------------------------------

size_t ts::CASSelectionArgs::addMatchingPIDs(PIDSet &pids, const CAT &cat, Report &report) const
{
	return addMatchingPIDs(pids, cat.descs, cat.tableId(), report);
}

size_t ts::CASSelectionArgs::addMatchingPIDs(PIDSet &pids, const PMT &pmt, Report &report) const
{
	size_t pid_count = addMatchingPIDs(pids, pmt.descs, pmt.tableId(), report);
	for (const auto &it : pmt.streams)
	{
		pid_count += addMatchingPIDs(pids, it.second.descs, pmt.tableId(), report);
	}
	return pid_count;
}


//----------------------------------------------------------------------------
// Analyze all CA_descriptors and locate all matching PID's.
//----------------------------------------------------------------------------

size_t ts::CASSelectionArgs::addMatchingPIDs(PIDSet &pids, const DescriptorList &dlist, TID tid, Report &report) const
{
	// Filter out useless cases.
	if ((tid == TID_CAT && !pass_emm) || (tid == TID_PMT && !pass_ecm) || (tid != TID_CAT && tid != TID_PMT))
	{
		return 0;
	}

	size_t pid_count = 0;

	if (cas_oper != 0)
	{
		// We must filter by operator id.
		// Collect all known forms of operator ids.
		PIDOperatorSet pidop;
		pidop.addAllOperators(dlist, tid == TID_CAT);

		// Loop on all collected PID and filter by operator id.
		for (const auto &it : pidop)
		{
			if (operatorMatch(it.oper) && casMatch(it.cas_id))
			{
				pids.set(it.pid);
				pid_count++;
				report.verbose(u"Filtering %s PID %d (0x%X)", { tid == TID_CAT ? u"EMM" : u"ECM", it.pid, it.pid });
			}
		}
	}
	else
	{
		// No filtering by operator, loop on all CA descriptors.
		for (size_t index = dlist.search(DID_CA); index < dlist.count(); index = dlist.search(DID_CA, index + 1))
		{
			const uint8_t *desc = dlist[index]->payload();
			const size_t size = dlist[index]->payloadSize();
			if (size >= 4)
			{
				// Get CA_system_id and ECM/EMM PID
				const uint16_t sysid = GetUInt16(desc);
				const PID pid = GetUInt16(desc + 2) & 0x1FFF;
				// Add ECM/EMM PID if it matches the required CAS id
				if (casMatch(sysid))
				{
					pids.set(pid);
					pid_count++;
					report.verbose(u"Filtering %s PID %d (0x%X)", { tid == TID_CAT ? u"EMM" : u"ECM", pid, pid });
				}
			}
		}
	}

	return pid_count;
}
