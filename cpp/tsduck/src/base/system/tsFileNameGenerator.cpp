//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsFileNameGenerator.h"


//----------------------------------------------------------------------------
// Get the number of trailing digits in a string.
//----------------------------------------------------------------------------

size_t ts::FileNameGenerator::TrailingDigits(const UString &str)
{
	size_t count = 0;
	const size_t len = str.length();
	while (count < len && IsDigit(str[len - 1 - count]))
	{
		count++;
	}
	return count;
}


//----------------------------------------------------------------------------
// Initialize the name prefix and suffix.
// Return the number of trailing digits in prefix.
//----------------------------------------------------------------------------

size_t ts::FileNameGenerator::init(const fs::path &name_template)
{
	// Analyze the file name template to isolate segments.
	fs::path prefix(name_template);
	prefix.replace_extension();
	_name_prefix = prefix;
	_name_suffix = name_template.extension();

	// Compute number of existing digits at end of template head.
	const size_t width = TrailingDigits(_name_prefix);

	// If no pre-existing integer field at end of file name, make sure to add a punctuation.
	if (width == 0 && !_name_prefix.empty())
	{
		const UChar c = _name_prefix.back();
		if (c != u'-' && c != u'_' && c != u'.' && c != u'/' && c != u'\\')
		{
			_name_prefix += u"-";
		}
	}

	return width;
}


//----------------------------------------------------------------------------
// Reinitialize the file name generator in counter mode.
//----------------------------------------------------------------------------

void ts::FileNameGenerator::initCounter(const fs::path &name_template, size_t initial_counter, size_t counter_width)
{
	_counter_mode = true;
	_counter_value = initial_counter;
	_counter_width = std::max<size_t>(1, counter_width);

	const size_t width = init(name_template);

	if (width > 0)
	{
		// Use existing integer field as initial value.
		_counter_width = width;
		const size_t len = _name_prefix.length();
		_name_prefix.substr(len - width).toInteger(_counter_value);
		_name_prefix.resize(len - width);
	}
}


//----------------------------------------------------------------------------
// Generate a new file name.
//----------------------------------------------------------------------------

fs::path ts::FileNameGenerator::newFileName()
{
	return fs::path(UString::Format(u"%s%0*d%s", { _name_prefix, _counter_width, _counter_value++, _name_suffix }));
}
