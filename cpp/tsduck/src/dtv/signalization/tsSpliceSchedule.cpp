//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsDuckContext.h"
#include "tsSpliceSchedule.h"
#include "tsTS.h"

#define MY_XML_NAME u"splice_schedule"
#define MY_STD ts::Standards::SCTE


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

ts::SpliceSchedule::SpliceSchedule() :
	AbstractSignalization(MY_XML_NAME, MY_STD)
{
}


//----------------------------------------------------------------------------
// Reset all fields to default initial values.
//----------------------------------------------------------------------------

void ts::SpliceSchedule::clearContent()
{
	events.clear();
}


//----------------------------------------------------------------------------
// Full dump of utc_splice_time.
//----------------------------------------------------------------------------

ts::UString ts::SpliceSchedule::DumpSpliceTime(const DuckContext &duck, uint32_t value)
{
	return UString::Format(u"0x%X (%s, leap seconds %s)", { value, ToUTCTime(duck, value).format(Time::DATETIME), duck.useLeapSeconds() ? u"included" : u"ignored" });
}

//----------------------------------------------------------------------------
// Deserialize a SpliceSchedule command from binary data.
//----------------------------------------------------------------------------

int ts::SpliceSchedule::deserialize(const uint8_t *data, size_t size)
{
	// Clear object content, make it a valid empty object.
	clear();

	const uint8_t *const start = data;
	if (size < 1)
	{
		invalidate();
		return -1; // too short
	}

	// Numbere of splice events
	uint8_t spliceCount = data[0];
	data += 1; size -= 1;

	while (spliceCount > 0)
	{
		// Decode one event.
		Event ev;

		ev.event_id = GetUInt32(data);
		ev.canceled = (data[4] & 0x80) != 0;
		data += 5; size -= 5;

		if (!ev.canceled)
		{
			if (size < 1)
			{
				invalidate();
				return -1; // too short
			}

			ev.splice_out = (data[0] & 0x80) != 0;
			ev.program_splice = (data[0] & 0x40) != 0;
			ev.use_duration = (data[0] & 0x20) != 0;
			data++; size--;

			if (ev.program_splice)
			{
				// The complete program switches at a given time.
				if (size < 4)
				{
					invalidate();
					return -1; // too short
				}
				ev.program_utc = GetUInt32(data);
				data += 4; size -= 4;
			}
			else
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
					if (size < 5)
					{
						invalidate();
						return -1; // too short
					}
					ev.components_utc.insert(std::make_pair(GetUInt8(data), GetUInt32(data + 1)));
					data += 5; size -= 5;
				}
			}
			if (ev.use_duration)
			{
				if (size < 5)
				{
					invalidate();
					return -1; // too short
				}
				ev.auto_return = (data[0] & 0x80) != 0;
				ev.duration_pts = (uint64_t(data[0] & 0x01) << 32) | uint64_t(GetUInt32(data + 1));
				data += 5; size -= 5;
			}
			if (size < 4)
			{
				invalidate();
				return -1; // too short
			}
			ev.program_id = GetUInt16(data);
			ev.avail_num = data[2];
			ev.avails_expected = data[3];
			data += 4; size -= 4;
		}

		// Finally add the deserialized event in the list.
		events.push_back(ev);
		spliceCount--;
	}

	return int(data - start);
}


//----------------------------------------------------------------------------
// Serialize the SpliceSchedule command.
//----------------------------------------------------------------------------

void ts::SpliceSchedule::serialize(ByteBlock &data) const
{
	data.appendUInt8(uint8_t(events.size()));

	for (auto &ev : events)
	{
		data.appendUInt32(ev.event_id);
		data.appendUInt8(ev.canceled ? 0xFF : 0x7F);

		if (!ev.canceled)
		{
			data.appendUInt8((ev.splice_out ? 0x80 : 0x00) |
				(ev.program_splice ? 0x40 : 0x00) |
				(ev.use_duration ? 0x20 : 0x00) |
				0x1F);
			if (ev.program_splice)
			{
				data.appendUInt32(uint32_t(ev.program_utc));
			}
			else
			{
				data.appendUInt8(uint8_t(ev.components_utc.size()));
				for (auto &it : ev.components_utc)
				{
					data.appendUInt8(it.first);
					data.appendUInt32(uint32_t(it.second));
				}
			}
			if (ev.use_duration)
			{
				data.appendUInt8((ev.auto_return ? 0xFE : 0x7E) | uint8_t(ev.duration_pts >> 32));
				data.appendUInt32(uint32_t(ev.duration_pts));
			}
			data.appendUInt16(ev.program_id);
			data.appendUInt8(ev.avail_num);
			data.appendUInt8(ev.avails_expected);
		}
	}
}

//----------------------------------------------------------------------------
// Convert between actual UTC time and 32-bit SCTE 35 utc_splice_time.
//----------------------------------------------------------------------------

ts::Time ts::SpliceSchedule::ToUTCTime(const DuckContext &duck, uint32_t value)
{
	Time utc(Time::GPSEpoch + Second(value) * MilliSecPerSec);
	if (duck.useLeapSeconds())
	{
		utc -= Time::GPSEpoch.leapSecondsTo(utc) * MilliSecPerSec;
	}
	return utc;
}

uint32_t ts::SpliceSchedule::FromUTCTime(const DuckContext &duck, const Time &value)
{
	if (value < Time::GPSEpoch)
	{
		return 0;
	}
	Second utc = (value - Time::GPSEpoch) / MilliSecPerSec;
	if (duck.useLeapSeconds())
	{
		utc += Time::GPSEpoch.leapSecondsTo(value);
	}
	return uint32_t(utc);
}
