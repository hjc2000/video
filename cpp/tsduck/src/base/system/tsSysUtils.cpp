//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/#license
//
//----------------------------------------------------------------------------

#include "tsFileUtils.h"
#include "tsSysUtils.h"

#if defined(TS_WINDOWS)
#include "tsBeforeStandardHeaders.h"
#include <intrin.h>
#include <io.h>
#include <psapi.h>
#include "tsAfterStandardHeaders.h"
#elif defined(TS_LINUX)
#include "tsBeforeStandardHeaders.h"
#include <sys/resource.h>
#include <dlfcn.h>
#include "tsAfterStandardHeaders.h"
#elif defined(TS_MAC)
#include "tsBeforeStandardHeaders.h"
#include <sys/resource.h>
#include <mach/mach.h>
#include <mach/message.h>
#include <mach/kern_return.h>
#include <mach/task_info.h>
#include <libproc.h>
#include <dlfcn.h>
#include "tsAfterStandardHeaders.h"
#elif defined(TS_BSD)
#include "tsSysCtl.h"
#include "tsBeforeStandardHeaders.h"
#include <sys/user.h>
#include <sys/resource.h>
#include <kvm.h>
#include <signal.h>
#include <dlfcn.h>
#if defined(TS_FREEBSD)
#include <libprocstat.h>
#elif defined(TS_DRAGONFLYBSD)
#include <sys/kinfo.h>
#endif
#include "tsAfterStandardHeaders.h"
#endif

// Required link libraries under Windows.
#if defined(TS_WINDOWS) && defined(TS_MSC)
#pragma comment(lib, "psapi.lib")  // GetProcessMemoryInfo
#endif

//----------------------------------------------------------------------------
// Check if the current user is privileged (UNIX root, Windows administrator).
//----------------------------------------------------------------------------

bool ts::IsPrivilegedUser()
{
	#if defined(TS_WINDOWS)
	::SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	::PSID AdministratorsGroup;
	::BOOL ok = ::AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup);
	if (ok)
	{
		if (!::CheckTokenMembership(nullptr, AdministratorsGroup, &ok))
		{
			ok = false;
		}
		::FreeSid(AdministratorsGroup);
	}
	return ok;
	#else
	return ::geteuid() == 0;
	#endif
}

//----------------------------------------------------------------------------
// Ignore SIGPIPE. On UNIX systems: writing to a broken pipe returns an
// error instead of killing the process. On Windows systems: does nothing.
//----------------------------------------------------------------------------

void ts::IgnorePipeSignal()
{
	#if !defined(TS_WINDOWS)
	::signal(SIGPIPE, SIG_IGN);
	#endif
}


//----------------------------------------------------------------------------
// Check if the standard input/output/error is a terminal.
//----------------------------------------------------------------------------

#if defined(TS_WINDOWS)
namespace
{
	// On Windows, only the DOS and PowerShell consoles are considered as terminal.
	// We also want to recognize as terminals the Cygwin and Msys consoles (mintty).
	bool StdHandleIsATerminal(::DWORD ns)
	{
		const ::HANDLE handle = ::GetStdHandle(ns);
		switch (::GetFileType(handle))
		{
		case FILE_TYPE_CHAR:
			{
				// A native console (DOS or PowerShell).
				return true;
			}
		case FILE_TYPE_PIPE:
			{
				// Check if associated file name matches Cygwin or Msys pty name.
				// With mintty, the standard devices are named pipes. With Cygwin,
				// the name starts with \cygwin. With Msys, the name starts with \msys.
				// Then, if the device is the mintty console, the name contains -pty.
				// For actual pipes, -pty is replaced by -pipe.
				return false;
			}
		default:
			{
				// Cannot be a terminal.
				return false;
			}
		}
	}
}
#endif

bool ts::StdInIsTerminal()
{
	#if defined(TS_WINDOWS)
	return StdHandleIsATerminal(STD_INPUT_HANDLE);
	#else
	return ::isatty(STDIN_FILENO);
	#endif
}

bool ts::StdOutIsTerminal()
{
	#if defined(TS_WINDOWS)
	return StdHandleIsATerminal(STD_OUTPUT_HANDLE);
	#else
	return ::isatty(STDOUT_FILENO);
	#endif
}

bool ts::StdErrIsTerminal()
{
	#if defined(TS_WINDOWS)
	return StdHandleIsATerminal(STD_ERROR_HANDLE);
	#else
	return ::isatty(STDERR_FILENO);
	#endif
}


//----------------------------------------------------------------------------
// Get the name of a class from the @c type_info of an object.
//----------------------------------------------------------------------------

#if defined(TS_GCC)
#include <cxxabi.h>
#endif

ts::UString ts::ClassName(const std::type_info &info)
{
	UString name;
	const char *const rtti = info.name();
	if (rtti != nullptr)
	{
		// By default, use the plain RTTI name. Not always a pretty name.
		name.assignFromUTF8(rtti);
		#if defined(TS_GCC)
		// With gcc and clang, this is a C++ mangled name.
		// Demangle it using the portable C++ ABI library.
		int status = 0;
		char *const demangled = abi::__cxa_demangle(rtti, nullptr, nullptr, &status);
		if (demangled != nullptr)
		{
			name.assignFromUTF8(demangled);
			::free(demangled);
		}
		#endif
		// Cleanup various initial decoration, depending on compiler.
		if (name.startWith(u"class "))
		{
			name.erase(0, 6);
		}
		// MSC: `anonymous namespace'::
		// GCC: (anonymous namespace)::
		if (name.find(u"anonymous namespace") == 1 && name.find(u"::") == 21)
		{
			name.erase(0, 23);
		}
	}
	return name;
}
