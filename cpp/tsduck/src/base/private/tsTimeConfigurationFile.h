//-----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//-----------------------------------------------------------------------------
//!
//!  @file
//!  Declare a singleton for the TSDuck time configuration file.
//!
//-----------------------------------------------------------------------------

#pragma once
#include "tsSingleton.h"
#include "tsTime.h"

namespace ts
{
	//!
	//! A singleton class for the TSDuck time configuration file.
	//! This class remains hidden inside the TSDuck library.
	//! Applications and plugins should use the class ts::Time.
	//!
	class TimeConfigurationFile
	{
	private:
		TimeConfigurationFile(TimeConfigurationFile &&) = delete;
		TimeConfigurationFile(const TimeConfigurationFile &) = delete;
		TimeConfigurationFile &operator=(TimeConfigurationFile &&) = delete;
		TimeConfigurationFile &operator=(const TimeConfigurationFile &) = delete;

	public:
		static TimeConfigurationFile &Instance();

	private:
		static TimeConfigurationFile *volatile _instance;
		static std::once_flag _once_flag; TimeConfigurationFile();
		static void CleanupSingleton();

	public:
		//!
		//! Get the number of leap seconds between two UTC dates.
		//! @param [in] start Start UTC date.
		//! @param [in] end End UTC date.
		//! @return The number of leap seconds between @a start and @a end.
		//! Return zero if @a start is after @a end.
		//!
		Second leapSeconds(const Time &start, const Time &end) const;

	private:
		// Definition of a <leap_second> entry.
		class LeapSecond
		{
		public:
			Time   after{};   // Insert leap seconds right after the second in this time.
			Second count = 0;  // Number of leap second to add (could be negative if necessary).

			// Constructor.
			LeapSecond() = default;

			// Comparison for sorting.
			bool operator<(const LeapSecond &other) const { return this->after < other.after; }
		};

		// TimeConfigurationFile private fields.
		Second initial_seconds = 0;               // Initial leap seconds before first leap second.
		std::vector<LeapSecond> leap_seconds{};  // Sorted list of defined leap seconds.
	};
}
