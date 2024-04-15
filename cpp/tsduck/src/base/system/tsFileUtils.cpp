//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/#license
//
//----------------------------------------------------------------------------

#include "tsErrCodeReport.h"
#include "tsFileUtils.h"
#include "tsMemory.h"
#include "tsSysUtils.h"

#if defined(TS_WINDOWS)
#include "tsBeforeStandardHeaders.h"
#include <userenv.h>
#include <memory.h>
#include <io.h>
#include <mmsystem.h>  // Memory management
#include <psapi.h>     // Process API
#include <comutil.h>   // COM utilities
#include <Shellapi.h>
#include "tsAfterStandardHeaders.h"
#else
#include "tsBeforeStandardHeaders.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include "tsAfterStandardHeaders.h"
#endif

// Required link libraries under Windows.
#if defined(TS_WINDOWS) && defined(TS_MSC)
#pragma comment(lib, "userenv.lib")  // GetUserProfileDirectory
#endif


//----------------------------------------------------------------------------
// Return a "vernacular" version of a file path.
//----------------------------------------------------------------------------

ts::UString ts::VernacularFilePath(const UString &path)
{
	UString vern(path);

	#if defined(TS_WINDOWS)
	// With Windows Linux Subsystem, the syntax "/mnt/c/" means "C:\"
	if (vern.length() >= 6 && vern.startWith(u"/mnt/") && IsAlpha(vern[5]) && (vern.length() == 6 || vern[6] == u'/'))
	{
		vern.erase(0, 4);
	}

	// With Cygwin, the syntax "/cygdrive/C/" means "C:\"
	if (vern.startWith(u"/cygdrive/"))
	{
		vern.erase(0, 9);
	}

	// On Windows, transform "/c/" pattern into "C:\" (typical on Msys).
	if (vern.length() >= 2 && vern[0] == u'/' && IsAlpha(vern[1]) && (vern.length() == 2 || vern[2] == u'/'))
	{
		vern[0] = ToUpper(vern[1]);
		vern[1] = u':';
		if (vern.length() == 2)
		{
			vern.append(u'\\');
		}
		else
		{
			vern[2] = u'\\';
		}
	}
	#endif

	// Normalize path separators.
	for (size_t i = 0; i < vern.length(); ++i)
	{
		if (vern[i] == u'/' || vern[i] == u'\\')
		{
			vern[i] = fs::path::preferred_separator;
		}
	}

	return vern;
}


//----------------------------------------------------------------------------
// Check if a file path is absolute (starting at a root of a file system).
//----------------------------------------------------------------------------

bool ts::IsAbsoluteFilePath(const ts::UString &path)
{
	#if defined(TS_WINDOWS)
	return path.startWith(u"\\\\") || (path.length() >= 3 && IsAlpha(path[0]) && path[1] == u':' && path[2] == u'\\');
	#else
	return !path.empty() && path[0] == u'/';
	#endif
}


//----------------------------------------------------------------------------
// Cleanup a file path.
//----------------------------------------------------------------------------

ts::UString ts::CleanupFilePath(const UString &path)
{
	// Include a trailing slash for subsequent substitutions.
	UString clean(path);
	clean.append(fs::path::preferred_separator);

	// Patterns to clean.
	const UString parent{ fs::path::preferred_separator, u'.', u'.', fs::path::preferred_separator };  //  /../
	const UString current{ fs::path::preferred_separator, u'.', fs::path::preferred_separator };       //  /./
	const UString dslash{ fs::path::preferred_separator, fs::path::preferred_separator };              //  //
	size_t pos = NPOS;
	size_t up = NPOS;

	// Remove redundant directory forms.
	while ((pos = clean.find(dslash)) != NPOS)
	{
		clean.erase(pos, 1);
	}
	while ((pos = clean.find(current)) != NPOS)
	{
		clean.erase(pos, 2);
	}

	// Remove redundant "parent/../".
	while ((pos = clean.find(parent)) != NPOS)
	{
		if (pos == 0)
		{
			// Path starting with "/../" -> can be removed.
			clean.erase(0, 3);
		}
		else if ((up = clean.rfind(fs::path::preferred_separator, pos - 1)) == NPOS)
		{
			// No "/" before "/../" -> the start of the string is the parent.
			clean.erase(0, pos + 4);
		}
		else
		{
			// Replace "/parent/../" by "/".
			clean.erase(up, pos - up + 3);
		}
	}

	// Remove trailing slashes.
	while (!clean.empty() && clean.back() == fs::path::preferred_separator)
	{
		clean.pop_back();
	}
	return clean;
}


//----------------------------------------------------------------------------
// Build the absolute form af a file path.
//----------------------------------------------------------------------------

ts::UString ts::AbsoluteFilePath(const UString &path, const UString &base)
{
	// Starting with a local form of the file path.
	UString full(VernacularFilePath(path));

	// If the path is already absolute, nothing to do.
	if (IsAbsoluteFilePath(full))
	{
		return CleanupFilePath(full);
	}
	else
	{
		return CleanupFilePath((base.empty() ? UString(fs::current_path(&ErrCodeReport())) : base) + fs::path::preferred_separator + full);
	}
}


//----------------------------------------------------------------------------
// Build a relative form of a file path, relative to a base directory.
//----------------------------------------------------------------------------

ts::UString ts::RelativeFilePath(const ts::UString &path, const ts::UString &base, ts::CaseSensitivity caseSensitivity, bool portableSlashes)
{
	// Build absolute file path of the target.
	UString target(AbsoluteFilePath(path));

	// Build absolute file path of the base directory, with a trailing path separator.
	UString ref(AbsoluteFilePath(base.empty() ? UString(fs::current_path(&ErrCodeReport())) : base));
	ref.append(fs::path::preferred_separator);

	// See how many leading characters are matching.
	size_t same = target.commonPrefixSize(ref, caseSensitivity);

	// Move backward right after the previous path separator to
	// get the length of the common directory parts
	while (same > 0 && target[same - 1] != fs::path::preferred_separator)
	{
		--same;
	}

	// If there is zero common character, no relative path is possible.
	// In that case, return the absolute path.
	// Note that this can normally happen on Windows only with paths
	// such as C:\foo\bar and D:\other. On Unix systems, there is at
	// least the root '/' in common.
	if (same > 0)
	{

		// There is a leading common part, remove it from target.
		target.erase(0, same);

		// For each remaining directory level in reference, insert a "../" in target.
		const UString up{ u'.', u'.', fs::path::preferred_separator };
		for (size_t i = same; i < ref.length(); ++i)
		{
			if (ref[i] == fs::path::preferred_separator)
			{
				target.insert(0, up);
			}
		}
	}

	// Convert portable slashes.
	if (portableSlashes && fs::path::preferred_separator != u'/')
	{
		target.substitute(fs::path::preferred_separator, u'/');
	}

	return target;
}


//----------------------------------------------------------------------------
// Find the last path separator in a name (including portable separator).
//----------------------------------------------------------------------------

namespace
{
	ts::UString::size_type LastPathSeparator(const ts::UString &path)
	{
		#if defined(TS_WINDOWS)
		// Also accept slash as path separator.
		ts::UString::size_type i = path.length();
		while (i > 0)
		{
			if (path[--i] == u'\\' || path[i] == u'/')
			{
				return i;
			}
		}
		return ts::NPOS;
		#else
		// Only one possibility.
		return path.rfind(fs::path::preferred_separator);
		#endif
	}
}


//----------------------------------------------------------------------------
// Return the directory name of a file path.
//----------------------------------------------------------------------------

ts::UString ts::DirectoryName(const UString &path)
{
	UString::size_type sep = LastPathSeparator(path);

	if (sep == NPOS)
	{
		return u".";               // No '/' in path => current directory
	}
	else if (sep == 0)
	{
		return path.substr(0, 1);  // '/' at beginning => root
	}
	else
	{
		return path.substr(0, sep);
	}
}


//----------------------------------------------------------------------------
// Return the base name of a file path.
//----------------------------------------------------------------------------

ts::UString ts::BaseName(const UString &path, const UString &suffix)
{
	const UString::size_type sep = LastPathSeparator(path);
	const UString base(path.substr(sep == NPOS ? 0 : sep + 1));
	const bool suffixFound = !suffix.empty() && base.endWith(suffix, FILE_SYSTEM_CASE_SENSITVITY);
	return suffixFound ? base.substr(0, base.size() - suffix.size()) : base;
}

//----------------------------------------------------------------------------
// Search an executable file.
//----------------------------------------------------------------------------

ts::UString ts::SearchExecutableFile(const UString &fileName, const UString &pathName)
{
	// Don't search if empty.
	if (fileName.empty())
	{
		return UString();
	}

	// Adjust file name with the executable suffix.
	UString name(fileName);
	if (!name.endWith(EXECUTABLE_FILE_SUFFIX, FILE_SYSTEM_CASE_SENSITVITY))
	{
		name.append(EXECUTABLE_FILE_SUFFIX);
	}

	// Executable mask at any level.
	const fs::perms exec = fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec;

	// If there is at least one path separator in the middle, there is a directory specified, don't search.
	if (LastPathSeparator(fileName) != NPOS)
	{
		// If the file does not exist or is not executable, not suitable.
		return fs::exists(name) && (fs::status(name, &ErrCodeReport()).permissions() & exec) != fs::perms::none ? name : UString();
	}

	// Search in the path.
	UStringList dirs;
	for (const auto &dir : dirs)
	{
		const UString full(dir + fs::path::preferred_separator + name);
		if (fs::exists(full) && (fs::status(full, &ErrCodeReport()).permissions() & exec) != fs::perms::none)
		{
			return full;
		}
	}

	// Not found.
	return UString();
}
