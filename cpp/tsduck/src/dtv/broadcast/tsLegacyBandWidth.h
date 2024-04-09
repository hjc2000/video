//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  @ingroup hardware
//!  Handling of legacy definitions for terrestrial modulation bandwidths.
//!
//!  Legacy issue: The bandwith type for DVB-T/T2 and ISDB-T used to be an enum type
//!  with a few values (BW_AUTO, BW_8_MHZ, etc.). This was a legacy from the Linux
//!  DVB API version 3. The bandwidth is now a 32-bit unsigned integer containing
//!  a value in Hz. The former enum values are redefined as constants.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsModulation.h"
#include "tsxml.h"

namespace ts
{

	class Args;

	constexpr BandWidth BW_AUTO = 0;          //!< Bandwidth automatically set (legacy symbol).
	constexpr BandWidth BW_1_712_MHZ = 1'712'000;  //!< 1.712 MHz bandwidth (DVB-T2 only, legacy symbol).
	constexpr BandWidth BW_5_MHZ = 5'000'000;  //!< 5 MHz bandwidth (DVB-T2 only, legacy symbol).
	constexpr BandWidth BW_6_MHZ = 6'000'000;  //!< 6 MHz bandwidth (legacy symbol).
	constexpr BandWidth BW_7_MHZ = 7'000'000;  //!< 7 MHz bandwidth (legacy symbol).
	constexpr BandWidth BW_8_MHZ = 8'000'000;  //!< 8 MHz bandwidth (legacy symbol).
	constexpr BandWidth BW_10_MHZ = 10'000'000; //!< 10 MHz bandwidth (DVB-T2 only, legacy symbol).

	//!
	//! Get the bandwidth value in Hz (deprecated).
	//! This is a legacy function, bandwidths are now integer values in Hz.
	//! @param [in] bw Bandwidth in Hz (or legacy bandwidth enumeration value).
	//! @return Bandwidth in Hz or zero if unknown.
	//!
	TSDUCKDLL inline uint32_t BandWidthValueHz(BandWidth bw) { return bw; }

	//!
	//! Get the bandwidth code from a value in Hz (deprecated).
	//! This is a legacy function, bandwidths are now integer values in Hz.
	//! @param [in] hz Bandwidth in Hz.
	//! @return Same bandwidth in Hz.
	//!
	TSDUCKDLL inline BandWidth BandWidthCodeFromHz(uint32_t hz) { return hz; }

	//!
	//! Convert a string containing a bandwidth value into an integer value in Hz.
	//! @param [out] bandwidth The bandwidth value. Unmodified in case of error.
	//! @param [in] str The string value containing either a integer value in Hz or a legacy enum value.
	//! @return True on success, false on invalid value.
	//!
	TSDUCKDLL bool LegacyBandWidthToHz(BandWidth &bandwidth, const UString &str);

	//!
	//! Get optional bandwidth parameter from an XML element, accepting legacy values.
	//! @param [out] bandwidth Returned value of the attribute in Hz. If the attribute is not present, the variable is reset.
	//! @param [in] element XML element containing the optional bandwidth.
	//! @param [in] attribute Name of the attribute.
	//! @return True on success, false on error.
	//!
	TSDUCKDLL bool GetLegacyBandWidth(std::optional<BandWidth> &bandwidth, const xml::Element *element, const UString &attribute);
}
