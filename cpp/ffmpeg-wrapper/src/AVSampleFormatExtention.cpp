#include "AVSampleFormatExtention.h"

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
