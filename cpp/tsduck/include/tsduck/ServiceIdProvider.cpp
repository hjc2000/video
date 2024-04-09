#include"tsduck/ServiceIdProvider.h"

using namespace video;

uint16_t video::ServiceIdProvider::GetServiceId(uint16_t desired_service_id)
{
	std::lock_guard l{ _lock };
	if (!_set[desired_service_id])
	{
		_set[desired_service_id] = 1;
		return desired_service_id;
	}

	return FindUnusedServiceId();
}

uint16_t video::ServiceIdProvider::FindUnusedServiceId()
{
	for (uint16_t i = 1; i < uint16_t(-1); i++)
	{
		if (!_set[i])
		{
			// 没被占用，标记为已占用，返回
			_set[i] = 1;
			return i;
		}
	}

	return 0;
}
