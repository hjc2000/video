#include<AVCalculate.h>

int64_t operator*(int64_t const int_num, AVRational const rational)
{
	return int_num * rational.num / rational.den;
}

int64_t operator*(AVRational const rational, int64_t const int_num)
{
	return int_num * rational;
}

AVRational operator*(AVRational r1, AVRational r2)
{
	return av_mul_q(r1, r2);
}

AVRational operator/(AVRational r1, AVRational r2)
{
	return av_div_q(r1, r2);
}

AVRational reciprocal(AVRational rational)
{
	AVRational ret{};
	ret.den = rational.num;
	ret.num = rational.den;
	return ret;
}
