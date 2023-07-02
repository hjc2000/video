#include <AVUtil.h>
#include <sstream>

#pragma region AVMediaType 的相关操作
std::string &FFmpeg::operator<<(std::string &str, FFmpeg::AVMediaType const type)
{
	str = std::string{ ::av_get_media_type_string(type) };
	return str;
}

std::ostream &FFmpeg::operator<<(std::ostream &ostream, FFmpeg::AVMediaType const type)
{
	std::string str;
	str << type;
	return ostream << str;
}

void FFmpeg::operator>>(std::string const &str, FFmpeg::AVMediaType &type)
{
	using FFmpeg::AVMediaType;
	/*
	enum AVMediaType {
	AVMEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as AVMEDIA_TYPE_DATA
	AVMEDIA_TYPE_VIDEO,
	AVMEDIA_TYPE_AUDIO,
	AVMEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
	AVMEDIA_TYPE_SUBTITLE,
	AVMEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
	AVMEDIA_TYPE_NB
};
	*/

	if (str == "audio")
		type = AVMediaType::AVMEDIA_TYPE_AUDIO;
	else if (str == "video")
		type = AVMediaType::AVMEDIA_TYPE_VIDEO;
	else if (str == "data")
		type = AVMediaType::AVMEDIA_TYPE_DATA;
	else if (str == "subtitle")
		type = AVMediaType::AVMEDIA_TYPE_SUBTITLE;
	else if (str == "nb")
		type = AVMediaType::AVMEDIA_TYPE_NB;
	else
		type = AVMediaType::AVMEDIA_TYPE_UNKNOWN;
}
#pragma endregion

#pragma region AVRational 的相关操作
std::string &FFmpeg::operator<<(std::string &str, FFmpeg::AVRational const rational)
{
	std::stringstream sstream;
	sstream << rational.num << "/" << rational.den << std::endl;
	sstream >> str;
	return str;
}

std::ostream &FFmpeg::operator<<(std::ostream &ostream, FFmpeg::AVRational const rational)
{
	return ostream << rational.num << "/" << rational.den;
}

double FFmpeg::operator*(int64_t const int_num, FFmpeg::AVRational const rational)
{
	return int_num * ::av_q2d(rational);
}

double FFmpeg::operator*(FFmpeg::AVRational const rational, int64_t const int_num)
{
	return int_num * ::av_q2d(rational);
}
#pragma endregion

#pragma region AVPixelFormat 的相关操作
std::string FFmpeg::get_pix_fmt_name(FFmpeg::AVPixelFormat pf)
{
	const char *name = ::av_get_pix_fmt_name(pf);
	if (name == nullptr)
		return "";
	else
		return name;
}

std::string &FFmpeg::operator<<(std::string &str, FFmpeg::AVPixelFormat pf)
{
	stringstream sstream;
	sstream << FFmpeg::get_pix_fmt_name(pf);
	sstream >> str;
	return str;
}

std::ostream &FFmpeg::operator<<(std::ostream &ostream, FFmpeg::AVPixelFormat pf)
{
	return ostream << FFmpeg::get_pix_fmt_name(pf);
}
#pragma endregion

