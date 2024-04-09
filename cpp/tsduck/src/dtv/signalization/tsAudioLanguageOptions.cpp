//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsAudioLanguageOptions.h"
#include "tsDuckContext.h"


//----------------------------------------------------------------------------
// Return a help string for the parameter syntax (static method)
//----------------------------------------------------------------------------

ts::UString ts::AudioLanguageOptions::GetHelpString()
{
	return
		u"The \"language-code\" is a 3-character string. The audio-type is optional, "
		u"its default value is zero. The \"location\" indicates how to locate the "
		u"audio stream. Its format is either \"Pn\" or \"An\". In the first case, "
		u"\"n\" designates a PID value and in the second case the audio stream number "
		u"inside the PMT, starting with 1. The default location is \"A1\", ie. the "
		u"first audio stream inside the PMT.\n";
}

//----------------------------------------------------------------------------
// Apply requested transformations on a PMT.
//----------------------------------------------------------------------------

bool ts::AudioLanguageOptionsVector::apply(DuckContext &duck, PMT &pmt, int severity) const
{
	bool ok = true;
	// Loop on all options
	for (const_iterator it = begin(); it != end(); ++it)
	{
		auto smi = pmt.streams.end();
		// Find audio stream in PMT
		if (it->locateByPID())
		{
			// Find the audio stream by PID in the PMT
			smi = pmt.streams.find(it->getPID());
			if (smi == pmt.streams.end())
			{
				duck.report().log(severity, u"audio PID %d (0x%X) not found in PMT", { it->getPID(), it->getPID() });
				ok = false;
			}
		}
		else
		{
			// Find audio stream number in PMT
			assert(it->getAudioStreamNumber() != 0);
			size_t audio_count = 0;
			smi = pmt.streams.begin();
			while (smi != pmt.streams.end())
			{
				if (smi->second.isAudio(duck) && ++audio_count >= it->getAudioStreamNumber())
				{
					break;
				}
				++smi;
			}
			if (smi == pmt.streams.end())
			{
				duck.report().log(severity, u"audio stream %d not found in PMT", { it->getAudioStreamNumber() });
				ok = false;
			}
		}
		// Update audio stream in PMT
		if (smi != pmt.streams.end())
		{
			// Remove any previous language descriptor
			smi->second.descs.removeByTag(DID_LANGUAGE);
			// Add a new one
			smi->second.descs.add(duck, ISO639LanguageDescriptor(*it));
		}
	}
	return ok;
}
