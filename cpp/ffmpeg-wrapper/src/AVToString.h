#pragma once
#include<include_ffmpeg.h>

std::string ToString(AVRational value);
std::string ToString(AVSampleFormat sample_format);
std::string ToString(AVPixelFormat pixel_format);
std::string ToString(AVMediaType media_type);