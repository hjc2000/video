//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsBinaryTable.h"
#include "tsCAT.h"
#include "tsDuckContext.h"
#include "tsPSIRepository.h"

#define MY_XML_NAME u"CAT"
#define MY_CLASS ts::CAT
#define MY_TID ts::TID_CAT
#define MY_PID ts::PID_CAT
#define MY_STD ts::Standards::MPEG

//----------------------------------------------------------------------------
// Constructors and destructors
//----------------------------------------------------------------------------

ts::CAT::CAT(uint8_t vers, bool cur) :
	AbstractDescriptorsTable(MY_TID, MY_XML_NAME, MY_STD, 0xFFFF, vers, cur)
{
}

ts::CAT::CAT(DuckContext &duck, const BinaryTable &table) :
	AbstractDescriptorsTable(duck, MY_TID, MY_XML_NAME, MY_STD, table)
{
}

ts::CAT::CAT(const ts::CAT &other) :
	AbstractDescriptorsTable(other)
{
}

ts::CAT::~CAT()
{
}


//----------------------------------------------------------------------------
// Inherited public methods
//----------------------------------------------------------------------------

bool ts::CAT::isPrivate() const
{
	return false; // MPEG-defined
}
