#pragma once
#include<cstdint>
#include<jccpp/Exception.h>
#include<queue>

/// <summary>
///		队列接口
/// </summary>
template<typename T>
class IQueue
{
public:
	virtual ~IQueue() {}

public:
	virtual uint64_t Count() const = 0;
	virtual T Dequeue() = 0;
	virtual bool TryDequeue(T &out) = 0;
	virtual void Enqueue(T obj) = 0;
	virtual void Clear() = 0;
};