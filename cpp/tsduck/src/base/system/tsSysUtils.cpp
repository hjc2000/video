//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/#license
//
//----------------------------------------------------------------------------

#include "tsFileUtils.h"
#include "tsSysUtils.h"
#include "tsTime.h"

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
// Return the name of the current application executable file.
//----------------------------------------------------------------------------

fs::path ts::ExecutableFile()
{
	UString path;

	#if defined(TS_WINDOWS)

	// Window implementation.
	std::array<::WCHAR, 2048> name;
	::DWORD length = ::GetModuleFileNameW(nullptr, name.data(), ::DWORD(name.size()));
	path = UString(name, length);

	#elif defined(TS_LINUX)

	// Linux implementation.
	// /proc/self/exe is a symbolic link to the executable.
	path = fs::weakly_canonical("/proc/self/exe", &ErrCodeReport());

	#elif defined(TS_MAC)

	// MacOS implementation.
	// The function proc_pidpath is documented as "private" and "subject to change".
	// Another option is _NSGetExecutablePath (not tested here yet).
	int length = 0;
	char name[PROC_PIDPATHINFO_MAXSIZE];
	if ((length = ::proc_pidpath(getpid(), name, sizeof(name))) < 0)
	{
		throw ts::Exception(u"proc_pidpath error", errno);
	}
	else
	{
		assert(length <= int(sizeof(name)));
		path.assignFromUTF8(name, length);
	}

	#elif defined(TS_FREEBSD) || defined(TS_DRAGONFLYBSD)

	// FreeBSD and DragonFlyBSD implementation.
	// We use the sysctl() MIB and the OID for the current executable is:
	path = SysCtrlString({ CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 }); // -1 means current process

	#elif defined(TS_NETBSD)

	// NetBSD implementation.
	// We use the sysctl() MIB and the OID for the current executable is:
	path = SysCtrlString({ CTL_KERN, KERN_PROC_ARGS, -1, KERN_PROC_PATHNAME }); // -1 means current process

	#elif defined(TS_OPENBSD)

	// OpenBSD implementation.
	// OpenBSD is the only OS without supported interface to get the current executable path,
	// giving invalid so-called "security reasons" for that. So, we try to guess it from the
	// original argv[0]. This is much less secure than having a supported interface. This is
	// why their "security reasons" are particularly stupid IMHO.

	ByteBlock argv_data(SysCtrlBytes({ CTL_KERN, KERN_PROC_ARGS, ::getpid(), KERN_PROC_ARGV }));
	if (argv_data.size() < sizeof(char *))
	{
		return fs::path();
	}
	char **argv = reinterpret_cast<char **>(argv_data.data());
	char *exe = argv[0];
	if (exe == nullptr)
	{
		return fs::path();
	}
	if (::strchr(exe, '/') != nullptr)
	{
		// A path is provided, resolve it.
		char *path8 = ::realpath(exe, nullptr);
		if (path8 != nullptr)
		{
			path.assignFromUTF8(path8);
			::free(path8);
		}
	}
	else
	{
		// A simple command name is provided, find it in the PATH.
		path = SearchExecutableFile(UString::FromUTF8(exe));
	}

	#else
	#error "ts::ExecutableFile not implemented on this system"
	#endif

	return path.empty() ? fs::path() : fs::weakly_canonical(path);
}


//----------------------------------------------------------------------------
//! Get the name of the executable or shared library containing the caller.
//----------------------------------------------------------------------------

fs::path ts::CallerLibraryFile()
{
	#if defined(TSDUCK_STATIC)

	// In case of static build, there is no shared library.
	// All code is in the main executable.
	return ExecutableFile();

	#elif defined(TS_MSC)

	// Window implementation.
	// Get return address of current function (in caller code).
	void *const ret = _ReturnAddress();
	// Get the module (DLL) into which this address can be found.
	::HMODULE handle = nullptr;
	if (::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, ::LPCWSTR(ret), &handle) == 0)
	{
		return fs::path();
	}
	else
	{
		std::array<::WCHAR, 2048> name;
		::DWORD length = ::GetModuleFileNameW(handle, name.data(), ::DWORD(name.size()));
		return UString(name, length);
	}
	#endif
}


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
// Get the CPU time of the process in milliseconds.
//----------------------------------------------------------------------------

ts::MilliSecond ts::GetProcessCpuTime()
{
	#if defined(TS_WINDOWS)

	::FILETIME creation_time, exit_time, kernel_time, user_time;
	if (::GetProcessTimes(::GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time) == 0)
	{
		throw ts::Exception(u"GetProcessTimes error", ::GetLastError());
	}
	return ts::Time::Win32FileTimeToMilliSecond(kernel_time) + ts::Time::Win32FileTimeToMilliSecond(user_time);

	#else

	::rusage usage;
	TS_ZERO(usage);
	if (::getrusage(RUSAGE_SELF, &usage) < 0)
	{
		throw ts::Exception(u"getrusage error", errno);
	}
	return MilliSecond(usage.ru_stime.tv_sec) * MilliSecPerSec +
		MilliSecond(usage.ru_stime.tv_usec) / MicroSecPerMilliSec +
		MilliSecond(usage.ru_utime.tv_sec) * MilliSecPerSec +
		MilliSecond(usage.ru_utime.tv_usec) / MicroSecPerMilliSec;

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
