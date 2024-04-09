#include"jccpp/TaskCompletionSignal.h"

using namespace std;

TaskCompletionSignal::TaskCompletionSignal(bool completed)
{
	_completed = completed;
}

TaskCompletionSignal::~TaskCompletionSignal()
{
	Dispose();
}

void TaskCompletionSignal::Dispose()
{
	if (_disposed)
	{
		return;
	}

	_disposed = true;

	// 让正在阻塞的 Wait 立刻停止阻塞。
	SetResult();
}

bool TaskCompletionSignal::IsCompleted()
{
	if (_disposed)
	{
		return true;
	}

	return _completed;
}

void TaskCompletionSignal::Wait()
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

void TaskCompletionSignal::SetResult()
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

void TaskCompletionSignal::Reset()
{
	std::lock_guard l(_mtx);
	_completed = false;
}
