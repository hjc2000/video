#include"strings.h"

int bcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n);
}

void bcopy(const void *src, void *dest, size_t n)
{
	memcpy(dest, src, n);
}

void bzero(void *s, size_t n)
{
	memset(s, 0, n);
}

void explicit_bzero(void *s, size_t n)
{
	volatile char *vs = (volatile char *)s;
	while (n)
	{
		*vs++ = 0;
		n--;
	}
}

char *index(const char *s, int c)
{
	return (char *)strchr(s, c);
}

char *rindex(const char *s, int c)
{
	return (char *)strrchr(s, c);
}

int ffs(int i)
{
	int bit;

	if (0 == i)
		return 0;

	for (bit = 1; !(i & 1); ++bit)
		i >>= 1;
	return bit;
}

int ffsl(long i)
{
	int bit;

	if (0 == i)
		return 0;

	for (bit = 1; !(i & 1); ++bit)
		i >>= 1;
	return bit;
}

int ffsll(long long i)
{
	int bit;

	if (0 == i)
		return 0;

	for (bit = 1; !(i & 1); ++bit)
		i >>= 1;
	return bit;
}

int strcasecmp(const char *s1, const char *s2)
{
	const unsigned char *u1 = (const unsigned char *)s1;
	const unsigned char *u2 = (const unsigned char *)s2;
	int result;

	while ((result = tolower(*u1) - tolower(*u2)) == 0 && *u1 != 0)
	{
		u1++;
		u2++;
	}

	return result;
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
	const unsigned char *u1 = (const unsigned char *)s1;
	const unsigned char *u2 = (const unsigned char *)s2;
	int result;

	for (; n != 0; n--)
	{
		result = tolower(*u1) - tolower(*u2);
		if (result)
			return result;
		if (*u1 == 0)
			return 0;
	}

	return 0;
}

int strcasecmp_l(const char *s1, const char *s2, _locale_t loc)
{
	const unsigned char *u1 = (const unsigned char *)s1;
	const unsigned char *u2 = (const unsigned char *)s2;
	int result;

	while ((result = _tolower_l(*u1, loc) - _tolower_l(*u2, loc)) == 0 && *u1 != 0)
	{
		u1++;
		u2++;
	}

	return result;
}

int strncasecmp_l(const char *s1, const char *s2, size_t n, _locale_t loc)
{
	const unsigned char *u1 = (const unsigned char *)s1;
	const unsigned char *u2 = (const unsigned char *)s2;
	int result;

	for (; n != 0; n--)
	{
		result = _tolower_l(*u1, loc) - _tolower_l(*u2, loc);
		if (result)
			return result;
		if (*u1 == 0)
			return 0;
	}
	return 0;
}
