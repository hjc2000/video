#pragma once
#include<functional>
#include<IDisposable.h>

namespace video
{
	class IPlayer :public IDisposable
	{
	public:
		~IPlayer() {}

	public:
		/// <summary>
		///		此调用不会阻塞，可以在回调函数中调用。
		///		* 派生此类时，此方法内部应该使用 recursive_mutex 加锁，保证线程安全。
		/// </summary>
		/// <param name="pause"></param>
		virtual void Pause(bool pause) = 0;
	};
}