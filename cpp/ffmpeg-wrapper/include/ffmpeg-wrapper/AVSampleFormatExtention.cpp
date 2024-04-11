#include "ffmpeg-wrapper/AVSampleFormatExtention.h"

using namespace video;
using namespace std;

ostream &operator<<(ostream &ostream, AVSampleFormat sf)
{
	return ostream << av_get_sample_fmt_name(sf);
}

string video::AVSampleFormatExtention::av_sample_format_to_string_with_endian(AVSampleFormat sf, bool little_endian)
{
	string base = av_get_sample_fmt_name(sf);
	if (little_endian)
		return base + "le";
	else
		return base + "be";
}

AVSampleFormat video::AVSampleFormatExtention::string_to_av_sample_format(string str)
{
	return ::av_get_sample_fmt(str.c_str());
}

int video::AVSampleFormatExtention::ParseRequiredSampleCount(std::string codec_name)
{
	int nb_samples = 1024;

	// 根据编码器来确定采样点数量。编码器对采样点数量有要求。
	if (std::string(codec_name) == "aac")
	{
		nb_samples = 1024;
	}

	return nb_samples;
}
