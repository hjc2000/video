#pragma once
#include<DateTime.h>

namespace video
{
	/// <summary>
	///		时间插值器。
	/// </summary>
	class TimeInterpolator
	{
	private:
		/// <summary>
		///		每次调用 SyncTime 方法就会捕获当前世界时间的毫秒数存到此字段。
		/// </summary>
		int64_t _time_at_sync_in_milliseconds = 0;

		/// <summary>
		///		步长较大的时间，在 SyncTime 方法中被设置。此时间需要被插值。
		/// </summary>
		int64_t _stepping_time_in_milliseconds = 0;

		bool _paused = true;

		/// <summary>
		///		当时间被暂停时，DeltaTimeInMilliseconds 方法返回此值，
		///		而不是返回当前世界时间减去 _time_at_sync_in_milliseconds
		/// </summary>
		int64_t _delta_time_when_pausing_in_milliseconds = 0;

		/// <summary>
		///		当前时刻（调用本方法的时刻）距离上一次 SyncTime 过去了多久。
		/// </summary>
		/// <returns></returns>
		int64_t DeltaTimeInMilliseconds()
		{
			if (_paused)
			{
				return _delta_time_when_pausing_in_milliseconds;
			}

			return DateTime::GetSteadyTimeInMilliseconds() - _time_at_sync_in_milliseconds;
		}

	public:
		/// <summary>
		///		将时间插值器的时间同步到 now_time_in_milliseconds，即当前时间。
		/// </summary>
		/// <param name="now_time_in_milliseconds">
		///		当前时间。
		///		时间插值器的时间将被同步到 now_time_in_milliseconds。
		/// </param>
		void SyncTime(int64_t now_time_in_milliseconds)
		{
			_paused = false;
			_time_at_sync_in_milliseconds = DateTime::GetSteadyTimeInMilliseconds();
			_stepping_time_in_milliseconds = now_time_in_milliseconds;
			_delta_time_when_pausing_in_milliseconds = 0;
		}

		int64_t InterpolatedTimeInMilliseconds()
		{
			return _stepping_time_in_milliseconds + DeltaTimeInMilliseconds();
		}

		void Pause()
		{
			_paused = true;

			/* 记录当前的时间差，然后直到取消暂停前，让 DeltaTimeInMilliseconds 方法一直返回
			* _delta_time_when_pausing_in_milliseconds。
			*/
			_delta_time_when_pausing_in_milliseconds = DateTime::GetSteadyTimeInMilliseconds() - _time_at_sync_in_milliseconds;
		}
	};
}