//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsDuckContext.h"
#include "tsSpliceInsert.h"
#include "tsTS.h"

#define MY_XML_NAME u"splice_insert"
#define MY_STD ts::Standards::SCTE


//----------------------------------------------------------------------------
// Default constructor.
//----------------------------------------------------------------------------

ts::SpliceInsert::SpliceInsert() :
	AbstractSignalization(MY_XML_NAME, MY_STD)
{
}


//----------------------------------------------------------------------------
// Reset all fields to default initial values.
//----------------------------------------------------------------------------

void ts::SpliceInsert::clearContent()
{
	event_id = 0;
	canceled = true;
	splice_out = false;
	immediate = false;
	program_splice = false;
	use_duration = false;
	program_pts = INVALID_PTS;
	components_pts.clear();
	duration_pts = INVALID_PTS;
	auto_return = false;
	program_id = 0;
	avail_num = 0;
	avails_expected = 0;
}


//----------------------------------------------------------------------------
// Adjust PTS time values using the "PTS adjustment" field from a splice
// information section.
//----------------------------------------------------------------------------

void ts::SpliceInsert::adjustPTS(uint64_t adjustment)
{
	// Ignore null or invalid adjustment. And cancelation or immediate commands have no time.
	if (adjustment == 0 || adjustment > PTS_DTS_MASK || canceled || immediate)
	{
		return;
	}

	// Adjust program splice time.
	if (program_splice && program_pts.has_value() && program_pts.value() <= PTS_DTS_MASK)
	{
		program_pts = (program_pts.value() + adjustment) & PTS_DTS_MASK;
	}

	// Adjust components splice times.
	if (!program_splice)
	{
		for (auto &it : components_pts)
		{
			if (it.second.has_value() && it.second.value() <= PTS_DTS_MASK)
			{
				it.second = (it.second.value() + adjustment) & PTS_DTS_MASK;
			}
		}
	}
}


//----------------------------------------------------------------------------
// Get the highest or lowest PTS value in the command.
//----------------------------------------------------------------------------

uint64_t ts::SpliceInsert::highestPTS() const
{
	uint64_t result = INVALID_PTS;
	if (!canceled && !immediate)
	{
		// Check program splice time.
		if (program_splice && program_pts.has_value() && program_pts.value() <= PTS_DTS_MASK)
		{
			result = program_pts.value();
		}
		// Check components splice times.
		if (!program_splice)
		{
			for (auto &it : components_pts)
			{
				if (it.second.has_value() && it.second.value() <= PTS_DTS_MASK && (result == INVALID_PTS || it.second.value() > result))
				{
					result = it.second.value();
				}
			}
		}
	}
	return result;
}

uint64_t ts::SpliceInsert::lowestPTS() const
{
	uint64_t result = INVALID_PTS;
	if (!canceled && !immediate)
	{
		// Check program splice time.
		if (program_splice && program_pts.has_value() && program_pts.value() <= PTS_DTS_MASK)
		{
			result = program_pts.value();
		}
		// Check components splice times.
		if (!program_splice)
		{
			for (auto &it : components_pts)
			{
				if (it.second.has_value() && it.second.value() <= PTS_DTS_MASK && (result == INVALID_PTS || it.second.value() < result))
				{
					result = it.second.value();
				}
			}
		}
	}
	return result;
}

//----------------------------------------------------------------------------
// Deserialize a SpliceInsert command from binary data.
//----------------------------------------------------------------------------

int ts::SpliceInsert::deserialize(const uint8_t *data, size_t size)
{
	// Clear object content, make it a valid empty object.
	clear();

	const uint8_t *const start = data;
	if (size < 5)
	{
		invalidate();
		return -1; // too short
	}

	event_id = GetUInt32(data);
	canceled = (data[4] & 0x80) != 0;
	data += 5; size -= 5;

	if (canceled)
	{
		return int(data - start);  // end of command
	}
	if (size < 1)
	{
		invalidate();
		return -1; // too short
	}

	splice_out = (data[0] & 0x80) != 0;
	program_splice = (data[0] & 0x40) != 0;
	use_duration = (data[0] & 0x20) != 0;
	immediate = (data[0] & 0x10) != 0;
	data++; size--;

	if (program_splice && !immediate)
	{
		// The complete program switches at a given time.
		const int s = program_pts.deserialize(data, size);
		if (s < 0)
		{
			invalidate();
			return -1; // invalid
		}
		data += s; size -= s;
	}
	if (!program_splice)
	{
		// Program components switch individually.
		if (size < 1)
		{
			invalidate();
			return -1; // too short
		}
		size_t count = data[0];
		data++; size--;
		while (count-- > 0)
		{
			if (size < 1)
			{
				invalidate();
				return -1; // too short
			}
			const uint8_t ctag = data[0];
			data++; size--;
			SpliceTime pts;
			if (!immediate)
			{
				const int s = pts.deserialize(data, size);
				if (s < 0)
				{
					invalidate();
					return -1; // invalid
				}
				data += s; size -= s;
			}
			components_pts.insert(std::make_pair(ctag, pts));
		}
	}
	if (use_duration)
	{
		if (size < 5)
		{
			invalidate();
			return -1; // too short
		}
		auto_return = (data[0] & 0x80) != 0;
		duration_pts = (uint64_t(data[0] & 0x01) << 32) | uint64_t(GetUInt32(data + 1));
		data += 5; size -= 5;
	}
	if (size < 4)
	{
		invalidate();
		return -1; // too short
	}
	program_id = GetUInt16(data);
	avail_num = data[2];
	avails_expected = data[3];
	data += 4; size -= 4;

	return int(data - start);
}


//----------------------------------------------------------------------------
// Serialize the SpliceInsert command.
//----------------------------------------------------------------------------

void ts::SpliceInsert::serialize(ByteBlock &data) const
{
	data.appendUInt32(event_id);
	data.appendUInt8(canceled ? 0xFF : 0x7F);

	if (!canceled)
	{
		data.appendUInt8((splice_out ? 0x80 : 0x00) |
			(program_splice ? 0x40 : 0x00) |
			(use_duration ? 0x20 : 0x00) |
			(immediate ? 0x10 : 0x00) |
			0x0F);
		if (program_splice && !immediate)
		{
			program_pts.serialize(data);
		}
		if (!program_splice)
		{
			data.appendUInt8(uint8_t(components_pts.size()));
			for (auto &it : components_pts)
			{
				data.appendUInt8(it.first);
				if (!immediate)
				{
					it.second.serialize(data);
				}
			}
		}
		if (use_duration)
		{
			data.appendUInt8((auto_return ? 0xFE : 0x7E) | uint8_t(duration_pts >> 32));
			data.appendUInt32(uint32_t(duration_pts));
		}
		data.appendUInt16(program_id);
		data.appendUInt8(avail_num);
		data.appendUInt8(avails_expected);
	}
}
