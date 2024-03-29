#include "SDL_Timer.h"
#include<Exception.h>

using namespace video;

uint32_t video::SDL_Timer::static_callback(uint32_t interval, void *param)
{
	SDL_Timer *me = (SDL_Timer *)param;
	if (me->_callback_should_stop)
	{
		cout << "SDL_Timer 停止" << endl;
		me->_callback_has_stopped.SetResult();
		return 0;
	}

	uint32_t ret = me->_callback(interval);
	if (ret == 0)
	{
		// _callback 返回值为 0，说明用户想停止定时器。
		cout << "SDL_Timer 停止" << endl;
		me->_callback_has_stopped.SetResult();
		return 0;
	}

	return ret;
}

void video::SDL_Timer::Start(uint32_t interval_in_milliseconds)
{
	lock_guard l(_not_private_methods_lock);
	if (!_callback_has_stopped.IsCompleted())
	{
		/* _callback_has_stopped 没完成，说明当前有一个回调任务没被停下来，
		* 即有一个定时器正在运行。
		*/
		cout << "请先停止当前定时任务再启动新的" << endl;
		return;
	}

	if (!_callback)
	{
		throw jc::Exception("SDL_Timer 的回调函数不能为空");
	}

	// 到这里，条件满足了，可以开启定时器了。
	_callback_should_stop = false;
	_callback_has_stopped.Reset();

	SDL_AddTimer(interval_in_milliseconds, static_callback, this);
}
