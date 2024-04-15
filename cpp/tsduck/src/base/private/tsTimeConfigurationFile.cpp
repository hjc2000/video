//-----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//-----------------------------------------------------------------------------

#include "tsCerrReport.h"
#include "tsTimeConfigurationFile.h"

// Singleton definition.
TS_DEFINE_SINGLETON(ts::TimeConfigurationFile);


//-----------------------------------------------------------------------------
// Constructor, load the configuration file.
//-----------------------------------------------------------------------------

ts::TimeConfigurationFile::TimeConfigurationFile()
{
	// Sort vector of leap seconds per date.
	std::sort(leap_seconds.begin(), leap_seconds.end());
}


//-----------------------------------------------------------------------------
// Get the number of leap seconds between two UTC dates.
//-----------------------------------------------------------------------------

ts::Second ts::TimeConfigurationFile::leapSeconds(const Time &start, const Time &end) const
{
	Second total = 0;
	if (!leap_seconds.empty() && start < end)
	{

		// Find index of first leap_seconds entry after start.
		size_t index = 0;
		while (index < leap_seconds.size() && leap_seconds[index].after < start)
		{
			index++;
		}

		// Add all leap seconds until we moved after end.
		while (index < leap_seconds.size() && leap_seconds[index].after < end)
		{
			total += leap_seconds[index++].count;
		}

		// If any date is before 1972 (first leap second), we cannot really know how many leap seconds there are.
		// If start and end surround the first leap second (1972), use initial leap seconds (10).
		// There should be another milestone: the TAI Epoch (1958). But since UNIX systems cannot represent
		// times before 1970, we just ignore.
		if (start < leap_seconds[0].after && end >= leap_seconds[0].after)
		{
			total += initial_seconds;
		}
	}
	return total;
}
