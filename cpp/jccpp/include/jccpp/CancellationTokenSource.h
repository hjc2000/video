#pragma once
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

	void Cancel();

public:
	/// <summary>
	///		是否需要取消
	/// </summary>
	/// <returns></returns>
	bool IsCancellationRequested() const;

	/// <summary>
	///		注册一个委托，当令牌取消时会被调用。可以多次调用注册多个委托。
	/// </summary>
	/// <param name="func"></param>
	/// <returns>返回一个 id，用来标识此次注册的委托。取消时可以用此 id 取消。</returns>
	uint64_t Register(std::function<void(void)> func);

	/// <summary>
	///		注销指定的取消时将被调用的委托。
	/// </summary>
	/// <param name="id">委托的 id</param>
	void Unregister(uint64_t id);
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
	bool IsCancellationRequested();
	shared_ptr<CancellationToken> Token() const;
	void Cancel();
};

class TaskCanceledException :public std::runtime_error
{
public:
	TaskCanceledException() :runtime_error(__func__) {}
	TaskCanceledException(const char *msg) :runtime_error(std::string(__func__) + " " + msg) {}
	TaskCanceledException(std::string msg) :runtime_error(std::string(__func__) + " " + msg) {}
};
