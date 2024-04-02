/*
MIT License
Copyright (c) 2019 win32ports
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#ifndef __STRINGS_H_868520A6_2F4E_4B49_AF4B_02B5A21CF6BB__
#define __STRINGS_H_868520A6_2F4E_4B49_AF4B_02B5A21CF6BB__

#ifndef _WIN32

#pragma message("this strings.h implementation is for Windows only!")

#else /* _WIN32 */

#ifdef __cplusplus
extern "C" {
	#endif /* __cplusplus */

	#include <string.h>
	#include <ctype.h>

	int bcmp(const void *s1, const void *s2, size_t n);
	void bcopy(const void *src, void *dest, size_t n);
	void bzero(void *s, size_t n);
	void explicit_bzero(void *s, size_t n);
	char *index(const char *s, int c);
	char *rindex(const char *s, int c);
	int ffs(int i);
	int ffsl(long i);
	int ffsll(long long i);

	#ifndef __MINGW32__

	int strcasecmp(const char *s1, const char *s2);
	int strncasecmp(const char *s1, const char *s2, size_t n);
	int strcasecmp_l(const char *s1, const char *s2, _locale_t loc);
	int strncasecmp_l(const char *s1, const char *s2, size_t n, _locale_t loc);

	#endif

	#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _WIN32 */

#endif /* __STRINGS_H_868520A6_2F4E_4B49_AF4B_02B5A21CF6BB__ */
