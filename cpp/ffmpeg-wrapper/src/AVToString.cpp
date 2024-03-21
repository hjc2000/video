#include<AVToString.h>
#include<Exception.h>
#include<include_ffmpeg.h>

std::string ToString(AVRational value)
{
	return std::format("{}/{}", value.num, value.den);
}

std::string ToString(AVSampleFormat sample_format)
{
	return av_get_sample_fmt_name(sample_format);
}

std::string ToString(AVPixelFormat pixel_format)
{
	const char *name = ::av_get_pix_fmt_name(pixel_format);
	if (name == nullptr)
	{
		cout << CODE_POS_STR << "未知的像素格式" << endl;
		throw jc::Exception();
	}

	return name;
}

std::string ToString(AVMediaType media_type)
{
	return std::string{ ::av_get_media_type_string(media_type) };
}