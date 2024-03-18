#pragma once
#include<chrono>
#include<chrono>
#include<cstdint>
#include<ctime>

class DateTime
{
public:
	/// <summary>
	///		用毫秒来获取当前的 UTC 时间。
	///		* 试了下，精度似乎只到 10ms，因为输出的时候最后一位永远是 0，而后三位表示的是毫秒。
	/// </summary>
	/// <returns>用毫秒表示的 UTC 时间</returns>
	static int64_t GetRealTimeInMilliseconds()
	{
		using namespace std;
		using namespace std::chrono;

		duration since_epoch = system_clock::now().time_since_epoch();
		int64_t millis = duration_cast<milliseconds>(since_epoch).count();
		return millis;
	}

	/**
	 * @brief 获取单调递增的时钟的时间。这个时间应该是来自系统持续运行时间，所以不会因为调整日期时间
	 * 而受影响，永远会单调递增。
	 *
	 * @return
	*/
	static int64_t GetSteadyTimeInMilliseconds()
	{
		using namespace std;
		using namespace std::chrono;

		duration now = steady_clock::now().time_since_epoch();
		int64_t now_in_milliseconds = duration_cast<milliseconds>(now).count();
		return now_in_milliseconds;
	}
};