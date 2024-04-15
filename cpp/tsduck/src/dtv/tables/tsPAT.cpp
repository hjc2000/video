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
