#pragma once
#include <global.h>

namespace FFmpeg
{
	class AVCodecContext : Wrapper<::AVCodecContext>
	{
	public:
		AVCodecContext(char *codecName);
	};
}
