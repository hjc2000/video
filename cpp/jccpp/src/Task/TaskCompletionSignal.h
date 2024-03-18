#pragma once
#include<atomic>
#include <condition_variable>
#include<functional>
#include<iostream>
#include <mutex>

class TaskCompletionSignal
{
public:
	TaskCompletionSignal(bool completed)
	{
		_completed = completed;
	}

	~TaskCompletionSignal()
	{
		Dispose();
	}

public:
	/// <summary>
	///		释放本对象。
	/// 
	///		释放后，本对象将不再具有阻塞的能力。同时，调用本方法时，正在阻塞的
	///		Wait 会立刻停止阻塞。
	/// </summary>
	void Dispose()
	{
		if (_disposed)
		{
			return;
		}

		_disposed = true;

		// 让正在阻塞的 Wait 立刻停止阻塞。
		SetResult();
	}

private:
	std::mutex _mtx;
	std::condition_variable _cv;
	std::atomic_bool _completed = false;
	std::atomic_bool _disposed = false;

public:
	/// <summary>
	///		如果 Dispose 被调用，本方法将永远返回 true，此时任务被锁定在完成状态，Reset
	///		方法不再具有效果。这种情况下，本对象的 Wait 将不再具有阻塞的能力。
	/// </summary>
	/// <returns></returns>
	bool IsCompleted()
	{
		if (_disposed)
		{
			return true;
		}

		return _completed;
	}

	/// <summary>
	///		等待，直到完成信号被触发。
	/// </summary>
	void Wait()
	{
		std::unique_lock<std::mutex> lock(_mtx);

		/* 条件变量的 wait 方法会立刻检查一次谓词，所以如果 IsCompleted 属性已经是
		* true 了，则不会阻塞，本方法会立刻返回。
		*/
		_cv.wait(lock, [&]()->bool
		{
			return IsCompleted();
		});
	}

	/// <summary>
	///		触发完成信号。
	/// </summary>
	void SetResult()
	{
		/* 这里需要竞争互斥锁，因为每次 SetResult 的调用都要保证 Wait 被释放一次，同时 Reset 方法
		* 也要互斥锁。这样可以防止 _completed 在这里被设置为 true 后另一个线程调用 Reset 又给设置
		* 成 false 了，于是 _cv.notify_all() 后，_cv 调用 lambda 表达式检查 IsCompleted 属性
		* 发现为 false，又继续阻塞了。于是本来 Wait 应该被释放一次，现在直接跳过了。
		*/
		std::lock_guard l(_mtx);
		_completed = true;
		_cv.notify_all(); // 通知所有等待的线程
	}

	/**
	 * @brief 重置完成状态。重置完了之后下一次 Wait 将会被阻塞。
	*/
	void Reset()
	{
		std::lock_guard l(_mtx);
		_completed = false;
	}
};
