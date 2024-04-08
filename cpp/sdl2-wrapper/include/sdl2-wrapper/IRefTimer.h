#pragma once
#include<cstdint>

namespace video
{
	class IRefTimer
	{
	public:
		virtual ~IRefTimer() {}

	public:
		/// <summary>
		///		获取当前的音频时间。单位毫秒。
		/// </summary>
		/// <returns></returns>
		virtual int64_t RefTime() = 0;
	};
}