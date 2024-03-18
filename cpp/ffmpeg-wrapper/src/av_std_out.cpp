#include<av_std_out.h>

std::ostream &operator<<(std::ostream &ostream, AVMediaType const media_type)
{
	return ostream << ToString(media_type);
}

std::ostream &operator<<(std::ostream &ostream, AVRational const rational)
{
	return ostream << ToString(rational);
}

std::ostream &operator<<(std::ostream &ostream, AVPixelFormat pixel_format)
{
	return ostream << ToString(pixel_format);
}
