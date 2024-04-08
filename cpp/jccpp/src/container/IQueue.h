#pragma once
#include<queue>
#include<cstdint>
#include<jccpp/Exception.h>

/// <summary>
///		队列接口
/// </summary>
template<typename T>
class IQueue
{
public:
	virtual ~IQueue() {}

public:
	virtual uint64_t Count() = 0;
	virtual T Dequeue() = 0;
	virtual bool TryDequeue(T &out) = 0;
	virtual void Enqueue(T obj) = 0;
	virtual void Clear() = 0;
};