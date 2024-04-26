#include "Timer.h"
#include<iostream>

using namespace video;
using namespace std;

video::Timer::Timer()
{
	SDL_Initializer::Initialize();
}

Timer::~Timer()
{
	Stop();
	cout << __func__ << endl;
}

void Timer::StopNoWait()
{
	lock_guard l(_not_private_methods_lock);
	_callback_should_stop = true;
}

void Timer::Stop()
{
	/* 这里使用的是原子量，_callback_has_stopped 的方法是线程安全的，所以不用加锁。*/
	_callback_should_stop = true;
	_callback_has_stopped.Wait();
}

uint32_t video::Timer::static_callback(uint32_t interval, void *param)
{
	Timer *me = (Timer *)param;
	if (me->_callback_should_stop)
	{
		cout << "Timer 停止" << endl;
		me->_callback_has_stopped.SetResult();
		return 0;
	}

	uint32_t ret = me->_callback(interval);
	if (ret == 0)
	{
		// _callback 返回值为 0，说明用户想停止定时器。
		cout << "Timer 停止" << endl;
		me->_callback_has_stopped.SetResult();
		return 0;
	}

	return ret;
}

void video::Timer::Start(uint32_t interval_in_milliseconds)
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
		throw std::runtime_error("Timer 的回调函数不能为空");
	}

	// 到这里，条件满足了，可以开启定时器了。
	_callback_should_stop = false;
	_callback_has_stopped.Reset();

	SDL_AddTimer(interval_in_milliseconds, static_callback, this);
}
