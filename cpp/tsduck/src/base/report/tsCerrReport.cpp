//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsCerrReport.h"
#include<tsUString.h>

// Define singleton instance
TS_DEFINE_SINGLETON(ts::CerrReport);

// Constructor.
ts::CerrReport::CerrReport()
{
	int severity = 0;
	setMaxSeverity(severity);
}

// Message logging method.
void ts::CerrReport::writeLog(int severity, const UString &msg)
{
	std::cerr << "* " << Severity::Header(severity) << msg << std::endl;
	std::cerr.flush();
}
