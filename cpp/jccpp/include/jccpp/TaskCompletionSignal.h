#pragma once
#include<atomic>
#include<condition_variable>
#include<functional>
#include<iostream>
#include<mutex>

class TaskCompletionSignal
{
public:
	TaskCompletionSignal(bool completed);
	~TaskCompletionSignal();

	/// <summary>
	///		释放后，本对象将不再具有阻塞的能力，正在阻塞的 Wait 会立刻停止阻塞。
	/// </summary>
	void Dispose();

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
	bool IsCompleted();

	/// <summary>
	///		等待，直到完成信号被触发。
	/// </summary>
	void Wait();

	/// <summary>
	///		触发完成信号。
	/// </summary>
	void SetResult();

	/// <summary>
	///		重置完成状态。重置完了之后下一次 Wait 将会被阻塞。
	/// </summary>
	void Reset();
};
