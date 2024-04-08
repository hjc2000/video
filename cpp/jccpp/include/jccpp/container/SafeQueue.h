#pragma once
#include<jccpp/container/IQueue.h>
#include<mutex>

/// <summary>
///		线程安全的队列，加了互斥锁。
/// </summary>
template<typename T>
class SafeQueue :public IQueue<T>
{
private:
	std::queue<T> _queue;
	std::mutex _lock;

public:
	/**
	 * @brief 队列中元素的数量。
	 * @return 
	*/
	uint64_t Count() override
	{
		std::lock_guard l(_lock);
		return _queue.size();
	}

	/**
	 * @brief 退队。
	 * @return
	 *
	 * @exception InvalidOperationException 如果队列为空，会抛出此异常。
	*/
	T Dequeue() override
	{
		std::lock_guard l(_lock);
		if (_queue.empty())
		{
			throw jc::InvalidOperationException("队列当前为空，无法退队");
		}

		T ret = _queue.front();
		_queue.pop();
		return ret;
	}

	/**
	 * @brief 尝试退队
	 * @param out 从队列里拿出来的元素会被赋值给 out，这要求 out 的类要实现赋值运算符。
	 * @return 退队成功返回 true，失败返回 false
	*/
	bool TryDequeue(T &out) override
	{
		std::lock_guard l(_lock);
		if (_queue.empty())
		{
			return false;
		}

		out = _queue.front();
		_queue.pop();
		return true;
	}

	/**
	 * @brief 入队
	 * @param obj
	*/
	void Enqueue(T obj) override
	{
		std::lock_guard l(_lock);
		_queue.push(obj);
	}

	/**
	 * @brief 清空队列。
	 *
	 * - 本类内部使用了 std 容器。从 std 容器中移除元素时，元素的析构函数会被调用。
	 * 如果这个元素是个指针，则指针的析构函数会被调用。又因为指针没有析构函数，所以编译器会将指针的泛型
	 * 容器实现为删除时不调用析构函数。
	 *
	 * - 删除容器内的指针时，指针指向的对象的析构函数不会被调用，即容器不会自动的指针执行 delete，所以
	 * 如果将指针放到容器中，然后撒手不管了，会造成内存泄漏。这种情况下可以用智能指针代替指针。
	*/
	void Clear() override
	{
		std::lock_guard l(_lock);

		/* 
		* 构造一个空的队列，然后让 _queue 与空队列交换。退出作用域后，empty_queue
		* 被销毁，所有元素都丢失。
		*
		* 这要求 T 类型必须不是在堆上创建的，否则会内存泄漏，因为本函数并不会 delete 每一个元素。
		*/
		std::queue<T> empty_queue;
		_queue.swap(empty_queue);
	}
};