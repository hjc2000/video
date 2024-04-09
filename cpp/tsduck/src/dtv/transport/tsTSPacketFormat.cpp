//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsTSPacketFormat.h"

//----------------------------------------------------------------------------
// Enumeration descriptions of TSPacketFormat.
//----------------------------------------------------------------------------

const ts::Enumeration ts::TSPacketFormatEnum({
	{u"autodetect", ts::TSPacketFormat::AUTODETECT},
	{u"TS",         ts::TSPacketFormat::TS},
	{u"M2TS",       ts::TSPacketFormat::M2TS},
	{u"RS204",      ts::TSPacketFormat::RS204},
	{u"duck",       ts::TSPacketFormat::DUCK},
	});

const ts::Enumeration ts::TSPacketFormatInputEnum({
	{u"autodetect", ts::TSPacketFormat::AUTODETECT},
	{u"TS",         ts::TSPacketFormat::TS},
	{u"M2TS",       ts::TSPacketFormat::M2TS},
	{u"RS204",      ts::TSPacketFormat::RS204},
	{u"duck",       ts::TSPacketFormat::DUCK},
	});

const ts::Enumeration ts::TSPacketFormatOutputEnum({
	{u"TS",         ts::TSPacketFormat::TS},
	{u"M2TS",       ts::TSPacketFormat::M2TS},
	{u"RS204",      ts::TSPacketFormat::RS204},
	{u"duck",       ts::TSPacketFormat::DUCK},
	});
