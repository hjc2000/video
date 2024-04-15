//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsBinaryTable.h"
#include "tsDuckContext.h"
#include "tsPAT.h"
#include "tsPSIBuffer.h"
#include "tsPSIRepository.h"
#include "tsxmlElement.h"

#define MY_XML_NAME u"PAT"
#define MY_CLASS ts::PAT
#define MY_TID ts::TID_PAT
#define MY_PID ts::PID_PAT
#define MY_STD ts::Standards::MPEG

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::PAT::PAT(uint8_t version_, bool is_current_, uint16_t ts_id_, PID nit_pid_) :
	AbstractLongTable(MY_TID, MY_XML_NAME, MY_STD, version_, is_current_),
	ts_id(ts_id_),
	nit_pid(nit_pid_),
	pmts()
{
}

ts::PAT::PAT(DuckContext &duck, const BinaryTable &table) :
	PAT(0, true, 0, PID_NULL)
{
	deserialize(duck, table);
}


//----------------------------------------------------------------------------
// Inherited public methods
//----------------------------------------------------------------------------

bool ts::PAT::isPrivate() const
{
	return false; // MPEG-defined
}

uint16_t ts::PAT::tableIdExtension() const
{
	return ts_id;
}


//----------------------------------------------------------------------------
// Clear the content of the table.
//----------------------------------------------------------------------------

void ts::PAT::clearContent()
{
	ts_id = 0;
	nit_pid = PID_NULL;
	pmts.clear();
}


//----------------------------------------------------------------------------
// Deserialization
//----------------------------------------------------------------------------

void ts::PAT::deserializePayload(PSIBuffer &buf, const Section &section)
{
	// Table id extension is TS id.
	ts_id = section.tableIdExtension();

	// The paylaod is a list of service_id/pmt_pid pairs
	while (buf.canRead())
	{
		const uint16_t id = buf.getUInt16();
		const uint16_t pid = buf.getPID();
		if (id == 0)
		{
			nit_pid = pid;
		}
		else
		{
			pmts[id] = pid;
		}
	}
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::PAT::serializePayload(BinaryTable &table, PSIBuffer &buf) const
{
	// Add the NIT PID once in the first section
	if (nit_pid != PID_NULL)
	{
		buf.putUInt16(0); // pseudo service_id
		buf.putPID(nit_pid);
	}

	// Add all services
	for (auto &it : pmts)
	{

		// If current section payload is full, close the current section.
		if (buf.remainingWriteBytes() < 4)
		{
			addOneSection(table, buf);
		}

		// Add current service entry into the PAT section
		buf.putUInt16(it.first);  // service_id
		buf.putPID(it.second);    // pmt pid
	}
}

//----------------------------------------------------------------------------
// XML serialization
//----------------------------------------------------------------------------

void ts::PAT::buildXML(DuckContext &duck, xml::Element *root) const
{
	root->setIntAttribute(u"version", version);
	root->setBoolAttribute(u"current", is_current);
	root->setIntAttribute(u"transport_stream_id", ts_id, true);
	if (nit_pid != PID_NULL)
	{
		root->setIntAttribute(u"network_PID", nit_pid, true);
	}
	for (auto &it : pmts)
	{
		xml::Element *e = root->addElement(u"service");
		e->setIntAttribute(u"service_id", it.first, true);
		e->setIntAttribute(u"program_map_PID", it.second, true);
	}
}


//----------------------------------------------------------------------------
// XML deserialization
//----------------------------------------------------------------------------

bool ts::PAT::analyzeXML(DuckContext &duck, const xml::Element *element)
{
	xml::ElementVector xservice;
	bool ok =
		element->getIntAttribute(version, u"version", false, 0, 0, 31) &&
		element->getBoolAttribute(is_current, u"current", false, true) &&
		element->getIntAttribute(ts_id, u"transport_stream_id", true, 0, 0x0000, 0xFFFF) &&
		element->getIntAttribute<PID>(nit_pid, u"network_PID", false, PID_NULL, 0x0000, 0x1FFF) &&
		element->getChildren(xservice, u"service", 0, 0x10000);

	for (auto it : xservice)
	{
		uint16_t id = 0;
		PID pid = PID_NULL;
		ok = it->getIntAttribute(id, u"service_id", true, 0, 0x0000, 0xFFFF) &&
			it->getIntAttribute<PID>(pid, u"program_map_PID", true, 0, 0x0000, 0x1FFF);
		if (ok)
		{
			pmts[id] = pid;
		}
	}

	return ok;
}
