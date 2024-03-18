#pragma once
#include<Exception.h>
#include<atomic>
#include<chrono>
#include<functional>
#include<iostream>
#include<map>
#include<memory>
#include<mutex>
#include<thread>

using std::shared_ptr;

class CancellationTokenSource;

/// <summary>
///		取消令牌
/// </summary>
class CancellationToken
{
private:
	friend class CancellationTokenSource;

	std::mutex _lock;
	std::atomic_bool _is_cancellation_request = false;
	std::map<uint64_t, std::function<void(void)>> _delegates;

	/// <summary>
	///		设置取消标志，同时执行所有通过 Register 方法注册的委托。
	///		* 设置取消标志是线程安全的，但是执行委托不是线程安全的。
	/// </summary>
	void Cancel()
	{
		_is_cancellation_request = true;

		// 执行列表中的每一个委托
		for (std::pair<uint64_t, std::function<void(void)>> const &pair : _delegates)
		{
			pair.second();
		}
	}

public:
	/// <summary>
	///		是否需要取消
	/// </summary>
	/// <returns></returns>
	bool IsCancellationRequested()
	{
		return _is_cancellation_request;
	}

	/// <summary>
	///		注册一个委托，当令牌取消时会被调用。可以多次调用注册多个委托。
	/// </summary>
	/// <param name="func"></param>
	/// <returns>返回一个 id，用来标识此次注册的委托。取消时可以用此 id 取消。</returns>
	uint64_t Register(std::function<void(void)> func)
	{
		std::lock_guard l(_lock);
		static uint64_t id = 0;
		uint64_t current_id = id++;
		_delegates[current_id] = func;
		return current_id;
	}

	/// <summary>
	///		取消委托。
	/// </summary>
	/// <param name="id">委托的 id</param>
	void Unregister(uint64_t id)
	{
		std::lock_guard l(_lock);
		_delegates.erase(id);
	}
};

/// <summary>
///		取消令牌源
/// </summary>
class CancellationTokenSource
{
private:
	std::atomic_bool _is_cancellation_request = false;
	shared_ptr<CancellationToken> _token{ new CancellationToken{} };
	std::mutex _lock;

public:
	/// <summary>
	///		是否需要取消
	/// </summary>
	/// <returns></returns>
	bool IsCancellationRequested()
	{
		std::lock_guard l(_lock);
		return _is_cancellation_request;
	}

	/// <summary>
	///		每次获取此属性都会构造一个新的 CancellationToken 返回。
	/// </summary>
	shared_ptr<CancellationToken> Token()
	{
		return _token;
	}

	/// <summary>
	///		取消所有从 Token 中获取的 CancellationToken。
	///		* 此方法幂等，且线程安全。
	/// </summary>
	void Cancel()
	{
		std::lock_guard l(_lock);

		// 已经取消过一次了就不要再取消了
		if (_is_cancellation_request) return;
		_is_cancellation_request = true;
		_token->Cancel();
	}
};

class TaskCanceledException :public jc::Exception
{
public:
	TaskCanceledException() :Exception(__func__) {}
	TaskCanceledException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
	TaskCanceledException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
};
