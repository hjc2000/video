#include"tsduck/PidProvider.h"

using namespace std;
using namespace video;

video::PidProvider::PidProvider()
{
	SetReservedPid();
}

uint16_t video::PidProvider::FindUnusedPid()
{
	for (uint16_t i = 100; i < 8192; i++)
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

void video::PidProvider::SetReservedPid()
{
	_set[0] = 1;
	_set[0x11] = 1;
}

uint16_t video::PidProvider::GetPid(uint16_t desired_pid)
{
	std::lock_guard l{ _lock };
	if (!_set[desired_pid])
	{
		_set[desired_pid] = 1;
		return desired_pid;
	}

	return FindUnusedPid();
}
