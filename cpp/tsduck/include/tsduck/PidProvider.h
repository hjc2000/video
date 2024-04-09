#pragma once
#include<mutex>
#include<stdint.h>
#include<tsTS.h>

namespace video
{
	class PidProvider
	{
	public:
		PidProvider();

	private:
		/// <summary>
		///		已分配的 PID 会置位。
		/// </summary>
		ts::PIDSet _set;
		std::mutex _lock;

		uint16_t FindUnusedPid();

		/// <summary>
		///		将保留的 PID 设置为占用。例如 PAT PID, SDT PID 这些是固定的，不允许自定义流使用的 PID。
		/// </summary>
		void SetReservedPid();

	public:
		/// <summary>
		///		传入期望的 PID，如果没被占用，则返回值就等于 desired_pid，如果已经被占用了，
		///		则返回一个没被占用的并占用它。
		/// </summary>
		/// <param name="desired_pid"></param>
		/// <returns></returns>
		uint16_t GetPid(uint16_t desired_pid);

		void ReturnPid(uint16_t pid)
		{
			std::lock_guard l{ _lock };
			_set[pid] = 0;
		}
	};
}