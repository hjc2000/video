/*每个头文件都要包含此文件，除非不依赖以下项目*/
#pragma once
extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <libavutil/log.h>
#include <libavutil/error.h>
#include <libavutil/avutil.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}
#include <memory>
#include <Wrapper.h>
#include <stdint.h>
#include <iostream>
using namespace std;
