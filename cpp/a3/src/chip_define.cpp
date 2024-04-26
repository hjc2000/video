#include "chip_define.h"

std::string ToString(chip_status status)
{
	switch (status)
	{
	case chip_status_badstatus:
		{
			return "badstatus";
		}
	case chip_status_fail_hw:
		{
			return "fail_hw";
		}
	case chip_status_fail_service:
		{
			return "fail_service";
		}
	case chip_status_fail_loader:
		{
			return "fail_loader";
		}
	case chip_status_unknown:
		{
			return "unknown";
		}
	case chip_status_waitcmd:
		{
			return "waitcmd";
		}
	case chip_status_running:
		{
			return "running";
		}
	default:
		{
			throw std::invalid_argument{ "未知的非法参数" };
		}
	}
}