//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Transport stream packet / file formats.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsEnumeration.h"

namespace ts
{

	class Args;

	//!
	//! Transport stream file formats.
	//!
	enum class TSPacketFormat
	{
		AUTODETECT,  //!< Try to detect format (read), default to TS.
		TS,          //!< Raw transport stream format.
		M2TS,        //!< Bluray compatible, 4-byte timestamp header before each TS packet (30-bit time stamp in PCR units).
		RS204,       //!< 204-byte packet with 16-byte trailing Reed-Solomon (ignored on input, zero place-holder on output).
		DUCK,        //!< Proprietary, 14-byte header before each TS packet (packet metadata).
	};

	//!
	//! Enumeration description of ts::TSPacketFormat.
	//!
	TSDUCKDLL extern const Enumeration TSPacketFormatEnum;

	//!
	//! Enumeration description of ts::TSPacketFormat as input file option.
	//!
	TSDUCKDLL extern const Enumeration TSPacketFormatInputEnum;

	//!
	//! Enumeration description of ts::TSPacketFormat as output file option.
	//!
	TSDUCKDLL extern const Enumeration TSPacketFormatOutputEnum;
}
