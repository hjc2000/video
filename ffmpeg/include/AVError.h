#pragma once
#include <string>
extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <libavutil/error.h>
}

namespace FFmpeg
{
	std::string error_code_to_str(int error_code);
}