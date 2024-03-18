#include<A3Device.h>
#include<UsbA3Device.h>
#include<stdarg.h>
#include<vatek_base.h>

vatek_debug_level m_debug_level = debug_level_warning;

void vatek_debug_set(vatek_debug_level level)
{
	m_debug_level = level;
}

void vatek_print(const char *fmt, ...)
{
	char sztmp[256];
	va_list arg;
	va_start(arg, fmt);
	vsprintf(sztmp, fmt, arg);
	va_end(arg);
}

vk_result vatek_version()
{
	return (vk_result)VATEK_VERSION;
}