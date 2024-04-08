#pragma once
#include<chrono>
#include<cstdint>
#include<ctime>

class DateTime
{
public:
	/// <summary>
	///		用毫秒来获取当前的 UTC 时间。
	///		试了下，精度似乎只到 10ms，因为输出的时候最后一位永远是 0。
	/// </summary>
	/// <returns>用毫秒表示的 UTC 时间</returns>
	static int64_t GetRealTimeInMilliseconds();

	/// <summary>
	///		获取单调递增的时钟的时间。这个时间可能是基于系统持续运行时间，
	///		所以不会因为调整日期时间而受影响，永远会单调递增。
	/// </summary>
	/// <returns></returns>
	static int64_t GetSteadyTimeInMilliseconds();
};