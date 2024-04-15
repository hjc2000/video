//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2022-2023, Paul Higgs
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsDescriptor.h"
#include "tsDuckContext.h"
#include "tsEVCTimingAndHRDDescriptor.h"
#include "tsPSIBuffer.h"
#include "tsPSIRepository.h"
#include "tsxmlElement.h"

#define MY_XML_NAME u"EVC_timing_and_HRD_descriptor"
#define MY_CLASS ts::EVCTimingAndHRDDescriptor
#define MY_DID ts::DID_MPEG_EXTENSION
#define MY_EDID ts::MPEG_EDID_EVC_TIM_HRD
#define MY_STD ts::Standards::MPEG

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::EVCTimingAndHRDDescriptor::EVCTimingAndHRDDescriptor() :
	AbstractDescriptor(MY_DID, MY_XML_NAME, MY_STD, 0)
{
}

void ts::EVCTimingAndHRDDescriptor::clearContent()
{
	hrd_management_valid = false;
	N_90khz.reset();
	K_90khz.reset();
	num_units_in_tick.reset();
}

ts::EVCTimingAndHRDDescriptor::EVCTimingAndHRDDescriptor(DuckContext &duck, const Descriptor &desc) :
	EVCTimingAndHRDDescriptor()
{
	deserialize(duck, desc);
}


//----------------------------------------------------------------------------
// This is an extension descriptor.
//----------------------------------------------------------------------------

ts::DID ts::EVCTimingAndHRDDescriptor::extendedTag() const
{
	return MY_EDID;
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::EVCTimingAndHRDDescriptor::serializePayload(PSIBuffer &buf) const
{
	const bool has_90kHz = N_90khz.has_value() && K_90khz.has_value();
	const bool info_present = num_units_in_tick.has_value();
	buf.putBit(hrd_management_valid);
	buf.putBits(0xFF, 6);
	buf.putBit(info_present);
	if (info_present)
	{
		buf.putBit(has_90kHz);
		buf.putBits(0xFF, 7);
		if (has_90kHz)
		{
			buf.putUInt32(N_90khz.value());
			buf.putUInt32(K_90khz.value());
		}
		buf.putUInt32(num_units_in_tick.value());
	}
}


//----------------------------------------------------------------------------
// Deserialization
//----------------------------------------------------------------------------

void ts::EVCTimingAndHRDDescriptor::deserializePayload(PSIBuffer &buf)
{
	hrd_management_valid = buf.getBool();
	buf.skipBits(6);
	const bool info_present = buf.getBool();
	if (info_present)
	{
		const bool has_90kHz = buf.getBool();
		buf.skipBits(7);
		if (has_90kHz)
		{
			N_90khz = buf.getUInt32();
			K_90khz = buf.getUInt32();
		}
		num_units_in_tick = buf.getUInt32();
	}
}

//----------------------------------------------------------------------------
// XML serialization
//----------------------------------------------------------------------------

void ts::EVCTimingAndHRDDescriptor::buildXML(DuckContext &duck, xml::Element *root) const
{
	root->setBoolAttribute(u"hrd_management_valid", hrd_management_valid);
	root->setOptionalIntAttribute(u"N_90khz", N_90khz);
	root->setOptionalIntAttribute(u"K_90khz", K_90khz);
	root->setOptionalIntAttribute(u"num_units_in_tick", num_units_in_tick);
}


//----------------------------------------------------------------------------
// XML deserialization
//----------------------------------------------------------------------------

bool ts::EVCTimingAndHRDDescriptor::analyzeXML(DuckContext &duck, const xml::Element *element)
{
	return element->getBoolAttribute(hrd_management_valid, u"hrd_management_valid", true) &&
		element->getOptionalIntAttribute(N_90khz, u"N_90khz") &&
		element->getOptionalIntAttribute(K_90khz, u"K_90khz") &&
		element->getOptionalIntAttribute(num_units_in_tick, u"num_units_in_tick");
}
