//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  A class which scans the services of a transport stream.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsTableHandlerInterface.h"
#include "tsSectionDemux.h"
#include "tsPAT.h"
#include "tsSDT.h"
#include "tsNIT.h"
#include "tsMGT.h"
#include "tsVCT.h"
#include "tsSafePtr.h"
#include<tsModulation.h>
#include<tsModulationArgs.h>

namespace ts {
    //!
    //! A class which scans the services of a transport stream.
    //! @ingroup mpeg
    //!
    class TSDUCKDLL TSScanner: private TableHandlerInterface
    {
        TS_NOBUILD_NOCOPY(TSScanner);
    public:
        //!
        //! Get the list of services.
        //! @param [out] services Returned list of services.
        //! @return True on success, false on error.
        //!
        bool getServices(ServiceList& services) const;

        //!
        //! Get the PAT of the transport stream.
        //! @param [out] pat Returned safe pointer to the PAT.
        //!
        void getPAT(SafePtr<PAT>& pat) const {pat = _pat;}

        //!
        //! Get the DVB SDT of the transport stream.
        //! @param [out] sdt Returned safe pointer to the DVB SDT.
        //!
        void getSDT(SafePtr<SDT>& sdt) const {sdt = _sdt;}

        //!
        //! Get the DVB NIT of the transport stream.
        //! @param [out] nit Returned safe pointer to the DVB NIT.
        //!
        void getNIT(SafePtr<NIT>& nit) const {nit = _nit;}

        //!
        //! Get the ATSC MGT of the transport stream.
        //! @param [out] mgt Returned safe pointer to the ATSC MGT.
        //!
        void getMGT(SafePtr<MGT>& mgt) const {mgt = _mgt;}

        //!
        //! Get the ATSC VCT of the transport stream.
        //! @param [out] vct Returned safe pointer to the ATSC VCT.
        //! The actual table is eiter a TVCT or a CVCT.
        //!
        void getVCT(SafePtr<VCT>& vct) const {vct = _vct;}

    private:
        DuckContext&   _duck;
        bool           _pat_only = false;
        bool           _completed = false;
        SectionDemux   _demux {_duck, this};
        ModulationArgs _tparams {};
        SafePtr<PAT>   _pat {};
        SafePtr<SDT>   _sdt {};
        SafePtr<NIT>   _nit {};
        SafePtr<MGT>   _mgt {};
        SafePtr<VCT>   _vct {};

        // Implementation of TableHandlerInterface.
        virtual void handleTable(SectionDemux&, const BinaryTable&) override;
    };
}
