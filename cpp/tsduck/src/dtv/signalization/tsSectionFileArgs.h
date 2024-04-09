//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Command line arguments for section file processing.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsSectionFile.h"
#include "tsReport.h"
#include "tsTime.h"
#include "tsEITOptions.h"

namespace ts {

    class Args;
    class DuckContext;

    //!
    //! Command line arguments for section file processing.
    //! @ingroup cmd
    //!
    class TSDUCKDLL SectionFileArgs
    {
    public:
        //!
        //! Constructor.
        //!
        SectionFileArgs() = default;

        // Public fields, by options.
        bool       pack_and_flush = false;             //!< Pack and flush incomplete tables before exiting.
        bool       eit_normalize = false;              //!< EIT normalization (ETSI TS 101 211).
        Time       eit_base_time {};                   //!< Last midnight reference for EIT normalization.
        EITOptions eit_options {EITOptions::GEN_ALL};  //!< EIT normalization options.

        //!
        //! Process the content of a section file according to the selected options.
        //! @param [in,out] file Section file to manipulate.
        //! @param [in,out] report Where to report errors.
        //! @return True on success, false on failure.
        //!
        bool processSectionFile(SectionFile& file, Report& report) const;
    };
}
