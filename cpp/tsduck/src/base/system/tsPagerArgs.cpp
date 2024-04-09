//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/#license
//
//----------------------------------------------------------------------------

#include "tsNullReport.h"
#include "tsPagerArgs.h"

//----------------------------------------------------------------------------
// Constructors and destructors.
//----------------------------------------------------------------------------

ts::PagerArgs::PagerArgs(bool pageByDefault, bool stdoutOnly) :
	page_by_default(pageByDefault),
	use_pager(pageByDefault),
	_pager(ts::OutputPager::DEFAULT_PAGER, stdoutOnly)
{
}

ts::PagerArgs::~PagerArgs()
{
	if (_pager.isOpen())
	{
		_pager.close(NULLREP);
	}
}

//----------------------------------------------------------------------------
// Return the output device for display.
//----------------------------------------------------------------------------

std::ostream &ts::PagerArgs::output(Report &report)
{
	if (use_pager && _pager.canPage() && (_pager.isOpen() || _pager.open(true, 0, report)))
	{
		// Paging is in use.
		return _pager;
	}
	else
	{
		// Cannot page, use standard output.
		return std::cout;
	}
}
