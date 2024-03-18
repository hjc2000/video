#pragma once
#include<include_ffmpeg.h>

int64_t operator*(int64_t const int_num, AVRational const rational);
int64_t operator*(AVRational const rational, int64_t const int_num);
AVRational operator*(AVRational r1, AVRational r2);

/**
 * @brief r1 / r2
 * @param r1 被除数
 * @param r2 除数
 * @return r1 / r2
*/
AVRational operator/(AVRational r1, AVRational r2);

/**
 * @brief 求倒数
 * @param rational
 * @return
*/
AVRational reciprocal(AVRational rational);

/**
 * @brief 将一个时间基的 pts 转换到另一个时间基。
 * @param in_pts
 * @param in_time_base
 * @param out_time_base
 * @return 转换到 out_time_base 后的 pts。
*/
inline int64_t ConvertTimeStamp(int64_t in_pts, AVRational in_time_base, AVRational out_time_base)
{
	// 时间戳转换 https://www.yuque.com/qiaodangyi/yrin4p/vn2y64n75vfgi3o3
	int64_t out_pts = av_rescale_q(in_pts, in_time_base, out_time_base);
	return out_pts;
}
