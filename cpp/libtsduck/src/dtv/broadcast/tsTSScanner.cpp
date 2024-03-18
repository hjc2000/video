//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsTSScanner.h"
#include "tsBinaryTable.h"
#include "tsTime.h"
#include "tsTSPacket.h"
#include "tsTVCT.h"
#include "tsCVCT.h"
#include "tsLogicalChannelNumbers.h"

#define BUFFER_PACKET_COUNT  10000 // packets


//----------------------------------------------------------------------------
// Get the list of services.
//----------------------------------------------------------------------------

bool ts::TSScanner::getServices(ServiceList& services) const
{
    services.clear();

    if (_pat.isNull()) {
        _duck.report().warning(u"No PAT found, services are unknown");
        return false;
    }

    if (_sdt.isNull() && _vct.isNull() && !_pat_only) {
        _duck.report().warning(u"No SDT or VCT found, services names are unknown");
        // do not return, collect service ids.
    }

    // Loop on all services in the PAT
    for (const auto& srv_it : _pat->pmts) {

        // Service id, PMT PID and TS id are extracted from the PAT
        Service srv;
        srv.setId(srv_it.first);
        srv.setPMTPID(srv_it.second);
        srv.setTSId(_pat->ts_id);

        // Original netw. id, service type, name and provider are extracted from the SDT.
        if (!_sdt.isNull()) {
            srv.setONId(_sdt->onetw_id);
            // Search service in the SDT
            const auto sit = _sdt->services.find(srv.getId());
            if (sit != _sdt->services.end()) {
                const uint8_t type = sit->second.serviceType(_duck);
                const UString name(sit->second.serviceName(_duck));
                const UString provider(sit->second.providerName(_duck));
                if (type != 0) {
                    srv.setTypeDVB(type);
                }
                if (!name.empty()) {
                    srv.setName(name);
                }
                if (!provider.empty()) {
                    srv.setProvider(provider);
                }
                srv.setCAControlled(sit->second.CA_controlled);
                srv.setEITpfPresent(sit->second.EITpf_present);
                srv.setEITsPresent(sit->second.EITs_present);
                srv.setRunningStatus(sit->second.running_status);
            }
        }

        // ATSC service descriptions are extracted from the VCT.
        if (!_vct.isNull()) {
            // Search service in the VCT
            const auto sit = _vct->findService(srv.getId());
            if (sit != _vct->channels.end()) {
                if (sit->second.service_type != 0) {
                    srv.setTypeATSC(sit->second.service_type);
                }
                if (!sit->second.short_name.empty()) {
                    srv.setName(sit->second.short_name);
                }
                srv.setCAControlled(sit->second.access_controlled);
                if (sit->second.major_channel_number > 0) {
                    // Major channel numbers start at 1.
                    srv.setMajorIdATSC(sit->second.major_channel_number);
                }
                // Minor channel number 0 is valid (means analog).
                srv.setMinorIdATSC(sit->second.minor_channel_number);
                srv.setHidden(sit->second.hidden);
            }
        }

        // Add new service definition in result
        services.push_back(srv);
    }

    // Logical channel numbers are extracted from the NIT.
    if (!_nit.isNull()) {
        LogicalChannelNumbers lcn_store(_duck);
        lcn_store.addFromNIT(*_nit);
        lcn_store.updateServices(services, Replacement::UPDATE);
    }

    return true;
}


//----------------------------------------------------------------------------
// Implementation of TableHandlerInterface.
//----------------------------------------------------------------------------

void ts::TSScanner::handleTable(SectionDemux&, const BinaryTable& table)
{
    _duck.report().debug(u"got table id 0x%X on PID 0x%X", {table.tableId(), table.sourcePID()});

    // Store known tables
    switch (table.tableId()) {

        case TID_PAT: {
            SafePtr<PAT> pat(new PAT(_duck, table));
            if (pat->isValid()) {
                _pat = pat;
                if (_pat->nit_pid != PID_NULL && _pat->nit_pid != PID_NIT) {
                    // Non standard NIT PID
                    _demux.removePID(PID_NIT);
                    _demux.addPID(pat->nit_pid);
                }
            }
            break;
        }

        case TID_SDT_ACT: {
            SafePtr<SDT> sdt(new SDT(_duck, table));
            if (sdt->isValid()) {
                _sdt = sdt;
            }
            break;
        }

        case TID_NIT_ACT: {
            SafePtr<NIT> nit(new NIT(_duck, table));
            if (nit->isValid()) {
                _nit = nit;
            }
            break;
        }

        case TID_MGT: {
            SafePtr<MGT> mgt(new MGT(_duck, table));
            if (mgt->isValid()) {
                _mgt = mgt;
                // Intercept TVCT and CVCT, they contain the service names.
                for (auto it = mgt->tables.begin(); it != mgt->tables.end(); ++it) {
                    switch (it->second.table_type) {
                        case ATSC_TTYPE_TVCT_CURRENT:
                        case ATSC_TTYPE_CVCT_CURRENT:
                            _demux.addPID(it->second.table_type_PID);
                            break;
                        default:
                            break;
                    }
                }
            }
            break;
        }

        case TID_TVCT: {
            SafePtr<VCT> vct(new TVCT(_duck, table));
            if (vct->isValid()) {
                _vct = vct;
            }
            break;
        }

        case TID_CVCT: {
            SafePtr<VCT> vct(new CVCT(_duck, table));
            if (vct->isValid()) {
                _vct = vct;
            }
            break;
        }

        default: {
            break;
        }
    }

    // When all tables are ready, stop collection
    _completed = !_pat.isNull() && (_pat_only || (!_sdt.isNull() && !_nit.isNull()) || (!_mgt.isNull() && !_vct.isNull()));
}
