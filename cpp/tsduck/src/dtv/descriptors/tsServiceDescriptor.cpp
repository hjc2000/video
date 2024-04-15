//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsDescriptor.h"
#include "tsDuckContext.h"
#include "tsNames.h"
#include "tsPSIBuffer.h"
#include "tsPSIRepository.h"
#include "tsServiceDescriptor.h"

#define MY_XML_NAME u"service_descriptor"
#define MY_CLASS ts::ServiceDescriptor
#define MY_DID ts::DID_SERVICE
#define MY_STD ts::Standards::DVB

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::ServiceDescriptor::ServiceDescriptor(uint8_t type, const UString &provider, const UString &name) :
	AbstractDescriptor(MY_DID, MY_XML_NAME, MY_STD, 0),
	service_type(type),
	provider_name(provider),
	service_name(name)
{
}

void ts::ServiceDescriptor::clearContent()
{
	service_type = 0;
	provider_name.clear();
	service_name.clear();
}

ts::ServiceDescriptor::ServiceDescriptor(DuckContext &duck, const Descriptor &desc) :
	ServiceDescriptor()
{
	deserialize(duck, desc);
}

ts::DescriptorDuplication ts::ServiceDescriptor::duplicationMode() const
{
	return DescriptorDuplication::REPLACE;
}


//----------------------------------------------------------------------------
// Binary serialization
//----------------------------------------------------------------------------

void ts::ServiceDescriptor::serializePayload(PSIBuffer &buf) const
{
	buf.putUInt8(service_type);
	buf.putStringWithByteLength(provider_name);
	buf.putStringWithByteLength(service_name);
}

void ts::ServiceDescriptor::deserializePayload(PSIBuffer &buf)
{
	service_type = buf.getUInt8();
	buf.getStringWithByteLength(provider_name);
	buf.getStringWithByteLength(service_name);
}
