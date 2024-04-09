//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsDuckContext.h"
#include "tsSectionFileArgs.h"


//----------------------------------------------------------------------------
// Process the content of a section file according to the selected options.
//----------------------------------------------------------------------------

bool ts::SectionFileArgs::processSectionFile(SectionFile &file, Report &report) const
{
	if (eit_normalize)
	{
		file.reorganizeEITs(eit_base_time, eit_options);
	}

	if (pack_and_flush)
	{
		const size_t packed = file.packOrphanSections();
		if (packed > 0)
		{
			report.verbose(u"packed %d incomplete tables, may be invalid", { packed });
		}
	}

	return true;
}
