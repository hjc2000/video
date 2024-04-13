#pragma once
#include<atomic>
#include<condition_variable>
#include<iostream>
#include<jccpp/IDisposable.h>
#include<jccpp/container/Queue.h>
#include<mutex>

namespace jc
{
	/// <summary>
	///		数据缓存。可以往里放入数据和取出数据。本质上是一个队列。当队列的元素数量超过上限时会丢弃最开始的数据。
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T>
	class DataChach :public IDisposable
	{
	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="max_count">必须大于等于 1，否则会抛出异常。</param>
		DataChach(int max_count)
		{
			if (max_count < 1)
			{
				throw jc::ArgumentException("max_count 必须大于等于 1");
			}

			_max_count = max_count;
		}

		~DataChach()
		{
			Dispose();
		}

		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;
		}

	private:
		std::atomic_bool _disposed = false;
		int _max_count;
		std::mutex _lock;
		Queue<T> _queue;
		std::condition_variable _can_tack_out;

	public:
		/// <summary>
		///		将数据放到内部队列的末尾。
		/// </summary>
		void PushBack(T item)
		{
			std::lock_guard l{ _lock };
			_queue.Enqueue(item);
			if (_queue.Count() > _max_count)
			{
				std::cout << CODE_POS_STR << "警告，数据队列元素超过最大值，丢弃最开始的数据。" << std::endl;
				_queue.Dequeue();
			}
		}

		/// <summary>
		///		取出数据。如果当前无数据可取会阻塞，直到有数据。取出后，数据将从内部队列中删除。
		/// </summary>
		/// <returns></returns>
		T TackOut()
		{
			std::unique_lock l{ _lock };
			_can_tack_out.wait(l, [&]()
			{
				if (_disposed)
				{
					return true;
				}

				return _queue.Count() > 0;
			});

			if (_disposed)
			{
				throw jc::ObjectDisposedException{};
			}

			return _queue.Dequeue();
		}
	};
}