//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsBinaryTable.h"
#include "tsDuckContext.h"
#include "tsPSIBuffer.h"
#include "tsPSIRepository.h"
#include "tsRST.h"

#define MY_XML_NAME u"RST"
#define MY_CLASS ts::RST
#define MY_TID ts::TID_RST
#define MY_PID ts::PID_RST
#define MY_STD ts::Standards::DVB

//----------------------------------------------------------------------------
// Definition of names for running status values.
//----------------------------------------------------------------------------

const ts::Enumeration ts::RST::RunningStatusNames({
	{u"undefined",   RS_UNDEFINED},
	{u"not-running", RS_NOT_RUNNING},
	{u"starting",    RS_STARTING},
	{u"pausing",     RS_PAUSING},
	{u"running",     RS_RUNNING},
	{u"off-air",     RS_OFF_AIR},
	});


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::RST::RST() :
	AbstractTable(MY_TID, MY_XML_NAME, MY_STD)
{
}

ts::RST::RST(DuckContext &duck, const BinaryTable &table) :
	RST()
{
	deserialize(duck, table);
}


//----------------------------------------------------------------------------
// Get the maximum size in bytes of the payload of sections of this table.
//----------------------------------------------------------------------------

size_t ts::RST::maxPayloadSize() const
{
	// Although a "private section" in the MPEG sense, the RST section is limited to 1024 bytes in ETSI EN 300 468.
	return MAX_PSI_SHORT_SECTION_PAYLOAD_SIZE;
}


//----------------------------------------------------------------------------
// Clear the content of the table.
//----------------------------------------------------------------------------

void ts::RST::clearContent()
{
	events.clear();
}


//----------------------------------------------------------------------------
// Deserialization
//----------------------------------------------------------------------------

void ts::RST::deserializePayload(PSIBuffer &buf, const Section &section)
{
	while (buf.canRead())
	{
		Event event;
		event.transport_stream_id = buf.getUInt16();
		event.original_network_id = buf.getUInt16();
		event.service_id = buf.getUInt16();
		event.event_id = buf.getUInt16();
		buf.skipReservedBits(5);
		buf.getBits(event.running_status, 3);
		events.push_back(event);
	}
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::RST::serializePayload(BinaryTable &table, PSIBuffer &buf) const
{
	for (auto &it : events)
	{
		buf.putUInt16(it.transport_stream_id);
		buf.putUInt16(it.original_network_id);
		buf.putUInt16(it.service_id);
		buf.putUInt16(it.event_id);
		buf.putBits(0xFF, 5);
		buf.putBits(it.running_status, 3);
	}
}
