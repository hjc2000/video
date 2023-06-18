#include <AVUtil.h>
#include <sstream>

#pragma region AVMediaType 的相关运算符重载
std::string &operator<<(std::string &str, FFmpeg::AVMediaType const type)
{
	str = std::string{::av_get_media_type_string(type)};
	return str;
}

std::ostream &operator<<(std::ostream &ostream, FFmpeg::AVMediaType const type)
{
	std::string str;
	str << type;
	return ostream << str;
}

void operator>>(std::string const &str, FFmpeg::AVMediaType &type)
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
#pragma endregion AVMediaType 的相关运算符重载

#pragma region AVRational 的相关运算符重载
std::string &operator<<(std::string &str, FFmpeg::AVRational const rational)
{
	std::stringstream sstream;
	sstream << rational.num << "/" << rational.den << std::endl;
	sstream >> str;
	return str;
}

std::ostream &operator<<(std::ostream &ostream, FFmpeg::AVRational const rational)
{
	return ostream << rational.num << "/" << rational.den;
}

double operator*(int64_t const int_num, FFmpeg::AVRational const rational)
{
	return int_num * ::av_q2d(rational);
}
double operator*(FFmpeg::AVRational const rational, int64_t const int_num)
{
	return int_num * ::av_q2d(rational);
}
#pragma endregion AVRational 的相关运算符重载
