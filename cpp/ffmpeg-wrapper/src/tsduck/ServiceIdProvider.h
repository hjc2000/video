#pragma once
#include<mutex>
#include<stdint.h>
#include<tsTS.h>

namespace video
{
	/// <summary>
	///		service_id 分配器。分配不冲突的 service_id。
	/// </summary>
	class ServiceIdProvider
	{
	public:
		/// <summary>
		///		获取一个 service_id。
		/// </summary>
		/// <param name="desired_service_id">
		///		期望的 service_id。如果没被占用，则返回值就等于 desired_service_id，并且会将 desired_service_id
		///		设置为已占用。如果已经被占用了，则会返回一个没被占用的。
		/// </param>
		/// <returns></returns>
		uint16_t GetServiceId(uint16_t desired_service_id);

		/// <summary>
		///		当一个 service_id 不再被占用时，可以归还。
		/// </summary>
		/// <param name="service_id"></param>
		void ReturnServiceId(uint16_t service_id)
		{
			std::lock_guard l{ _lock };
			_set[service_id] = 0;
		}

	private:
		std::bitset<uint16_t(-1) + 1> _set;
		std::mutex _lock;

		/// <summary>
		///		找到一个没被占用的 service_id。找到后返回该 service_id，并且会将它标记为占用。
		///		* 如果实在找不到，会返回 0.
		/// </summary>
		/// <returns></returns>
		uint16_t FindUnusedServiceId();
	};
}