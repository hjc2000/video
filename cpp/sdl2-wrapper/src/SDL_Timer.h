#pragma once
#include<SDL_EventGetter.h>
#include<SDL_Util.h>
#include<TaskCompletionSignal.h>
#include<atomic>
#include<iostream>

using namespace std;

namespace video
{
	/// <summary>
	///		* 本定时器支持反复停止和启动。
	///		* 本类线程安全，所有公共方法都有加锁。
	/// </summary>
	class SDL_Timer
	{
	public:// 生命周期
		SDL_Timer()
		{
			SDL_Initializer::Initialize();
		}

		~SDL_Timer()
		{
			Stop();
			cout << "~SDL_Timer" << endl;
		}

	private:
		/// <summary>
		///		* 此任务完成说明 static_callback 函数检测到 _callback_should_stop 标志位为真了。
		///		  然后 static_callback 函数会自己把定时器关了。 
		///		* Stop 函数会等待此任务的完成才返回。
		///		* 因为 SDL_Timer 对象刚构造的时候定时器不在运行，所以此任务初始时为 "已完成"。
		/// </summary>
		TaskCompletionSignal _callback_has_stopped{ true };

		/// <summary>
		///		static_callback 函数检测到此标志位为真后就不要执行任务了，
		///		直接设置 _callback_has_stopped 为完成，然后直接返回。
		/// </summary>
		atomic_bool _callback_should_stop = false;

		/// <summary>
		///		非私有的方法，即公共的和保护的，都要加锁。
		///		* 为什么保护的要加锁？因为保护的有可能是继承了基类的，而基类可能用公共方法
		///		  去调用保护的方法，导致本类的保护方法和公共方法一样，都有可能被外部在不同
		///		  线程中调用。
		/// </summary>
		recursive_mutex _not_private_methods_lock;

	private:
		static uint32_t static_callback(uint32_t interval, void *param);

	public:
		/// <summary>
		///		启动定时器。本定时器可以反复停止和启动。
		///		* 如果定时器已经处于启动状态，再次调用会直接返回。
		/// </summary>
		/// <param name="interval_in_milliseconds"></param>
		void Start(uint32_t interval_in_milliseconds);

		/// <summary>
		///		与 Stop 方法不同，本方法不会等待定时器回调结束后才返回，而是直接返回。
		///		* 本方法可以在定时器回调中使用。
		/// </summary>
		void StopNoWait()
		{
			lock_guard l(_not_private_methods_lock);
			_callback_should_stop = true;
		}

		/// <summary>
		///		本方法会阻塞，直到回调函数返回。
		/// </summary>
		void Stop()
		{
			/* 这里使用的是原子量，_callback_has_stopped 的方法是线程安全的，所以不用加锁。*/
			_callback_should_stop = true;
			_callback_has_stopped.Wait();
		}

		/// <summary>
		///		定时器回调函数。
		///		- 回调时会传入当前的时间间隔。函数需要返回下一次回调时的时间间隔。
		///		- 改变此字段必须是在定时器停止的状态下，否则会发生未知行为。
		/// </summary>
		/// <param name="interval">
		///		当前定时器的回调间隔是多少。
		///		- 如果是第一次回调，则是构造定时器对象时设置的值，如果不是第一次回调，
		///		  则是距离上一次回调过去了多久。
		///		- 单位：毫秒。
		/// </param>
		/// <return>
		///		返回下一次回调的时间间隔，单位毫秒。例如返回 1000，则下一次回调会在 1000ms 后。
		/// </return>
		function<uint32_t(uint32_t interval)> _callback = nullptr;
	};
}
