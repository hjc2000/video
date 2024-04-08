#pragma once
#include<sdl2-wrapper/SDL_EventGetter.h>
#include<sdl2-wrapper/SDL_Initializer.h>
#include<jccpp/TaskCompletionSignal.h>
#include<atomic>
#include<iostream>

namespace video
{
	/// <summary>
	///		本定时器支持反复停止和启动。
	///		本类线程安全。
	/// </summary>
	class SDL_Timer
	{
	public:
		SDL_Timer();
		~SDL_Timer();

	private:
		TaskCompletionSignal _callback_has_stopped{ true };
		std::atomic_bool _callback_should_stop = false;
		std::mutex _not_private_methods_lock;

		static uint32_t static_callback(uint32_t interval, void *param);

	public:
		/// <summary>
		///		启动定时器。本定时器可以反复停止和启动。
		///		如果定时器已经处于启动状态，再次调用会直接返回。
		/// </summary>
		/// <param name="interval_in_milliseconds"></param>
		void Start(uint32_t interval_in_milliseconds);

		/// <summary>
		///		与 Stop 方法不同，本方法不会等待定时器回调结束后才返回，而是直接返回。
		///		本方法可以在定时器回调中使用。
		/// </summary>
		void StopNoWait();

		/// <summary>
		///		本方法会阻塞，直到回调函数返回，所以在回调函数中使用会死锁。
		/// </summary>
		void Stop();

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
