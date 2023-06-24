#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS

extern "C"
{
	#include <libavutil/log.h>
	#include <libavutil/error.h>
	#include <libavutil/avutil.h>
	#include <libavformat/avio.h>
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/opt.h>
	#include <libavutil/timestamp.h>
	#include <libavfilter/buffersink.h>
	#include <libavfilter/buffersrc.h>
	#include <libavutil/channel_layout.h>
	#include <libavutil/pixdesc.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/samplefmt.h>
}
