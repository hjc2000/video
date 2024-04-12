#include"jccpp/CancellationTokenSource.h"

using namespace std;

void CancellationToken::Cancel()
{
	std::lock_guard l(_lock);
	_is_cancellation_request = true;

	// 执行列表中的每一个委托
	for (std::pair<uint64_t, std::function<void(void)>> const &pair : _delegates)
	{
		pair.second();
	}
}

bool CancellationToken::IsCancellationRequested() const
{
	return _is_cancellation_request;
}

uint64_t CancellationToken::Register(std::function<void(void)> func)
{
	std::lock_guard l(_lock);
	static uint64_t id = 0;
	uint64_t current_id = id++;
	_delegates[current_id] = func;
	return current_id;
}

void CancellationToken::Unregister(uint64_t id)
{
	std::lock_guard l(_lock);
	_delegates.erase(id);
}

bool CancellationTokenSource::IsCancellationRequested()
{
	std::lock_guard l(_lock);
	return _is_cancellation_request;
}

shared_ptr<CancellationToken> CancellationTokenSource::Token() const
{
	return _token;
}

void CancellationTokenSource::Cancel()
{
	std::lock_guard l(_lock);

	// 已经取消过一次了就不要再取消了
	if (_is_cancellation_request) return;
	_is_cancellation_request = true;
	_token->Cancel();
}
