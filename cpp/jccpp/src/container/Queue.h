#pragma once
#include<IQueue.h>

/// <summary>
///		* 本队列装的类必须实现拷贝构造函数和赋值运算符。
///		* 禁止使用 new 创建出对象后将对象的指针装入本队列，当退队或清空队列时会造成内存泄漏。
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
class Queue :public IQueue<T>
{
private:
	std::queue<T> _queue;

public:
	/// <summary>
	///		当前队列中元素的数量。
	/// </summary>
	/// <returns></returns>
	uint64_t Count() override
	{
		return _queue.size();
	}

	/// <summary>
	///		退队。如果队列为空，会抛出 InvalidOperationException
	/// </summary>
	/// <returns></returns>
	/// <exception cref="InvalidOperationException"></exception>
	T Dequeue() override
	{
		if (_queue.empty())
		{
			throw jc::InvalidOperationException("队列当前为空，无法退队");
		}

		T ret = _queue.front();
		_queue.pop();
		return ret;
	}

	/// <summary>
	///		尝试退队
	/// </summary>
	/// <param name="out">
	///		从队列里拿出来的元素会被赋值给 out，这要求 out 的类要实现赋值运算符。
	/// </param>
	/// <returns>退队成功返回 true，失败返回 false</returns>
	bool TryDequeue(T &out) override
	{
		if (_queue.empty())
		{
			return false;
		}

		out = _queue.front();
		_queue.pop();
		return true;
	}

	/// <summary>
	///		入队
	/// </summary>
	/// <param name="obj"></param>
	void Enqueue(T obj) override
	{
		_queue.push(obj);
	}

	/// <summary>
	///		清空队列。
	/// 
	///		* 本类内部使用了 std 容器。从 std 容器中移除元素时，元素的析构函数会被调用。
	/// 
	///		* 题外话：
	///		使用
	///		A* pa = new A[10];
	///		创建了 A 类的数组后，用
	///		delete[] pa;
	///		删除这个数组，A 类对象的析构函数都会被调用。
	/// 
	///		* 禁忌：
	///		禁止用 new 创建出对象后，将对象的指针装入本队列中。因为退队或清空队列后，
	///		被自动调用的 指针的析构函数 没什么作用，指针指向的对象并不会被销毁。
	///		可以使用 share_ptr 来代替指针。调用 share_ptr 的析构函数后，如果引用计数达到 0，
	///		则 share_ptr 指向的对象会被销毁。
	/// </summary>
	void Clear() override
	{
		/* 构造一个空的队列，然后让 _queue 与空队列交换。退出作用域后，empty_queue
		* 被销毁，所有元素都丢失。
		*
		* 这要求 T 类型必须不是在堆上创建的，否则会内存泄漏，因为本函数并不会 delete 每一个元素。
		*/
		std::queue<T> empty_queue;
		_queue.swap(empty_queue);
	}
};