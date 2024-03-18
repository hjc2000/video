#pragma once
#include<AVToString.h>

std::ostream &operator<<(std::ostream &ostream, AVMediaType const media_type);
std::ostream &operator<<(std::ostream &ostream, AVRational const rational);
std::ostream &operator<<(std::ostream &ostream, AVPixelFormat pf);
