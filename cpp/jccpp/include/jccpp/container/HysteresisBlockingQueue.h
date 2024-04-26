#pragma once
#include<condition_variable>
#include<iostream>
#include<jccpp/Exception.h>
#include<jccpp/IDisposable.h>
#include<jccpp/container/IQueue.h>
#include<jccpp/container/SafeQueue.h>
#include<jccpp/define.h>
#include<mutex>

namespace jc
{
	/// <summary>
	///		带有滞回特性的阻塞队列。
	///		- 队列满时，再往里送会阻塞，直到队列中的元素被消费到小于一定阈值才取消阻塞。
	///		- 队列空时，再往外拿会阻塞，直到队列中的元素大于一定阈值才取消阻塞。
	///		- 本队列线程安全。
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T>
	class HysteresisBlockingQueue :public IQueue<T>, public IDisposable
	{
	private:
		std::atomic_bool _disposed = false;

		/// <summary>
		///		* 被冲洗后入队会抛出异常。
		///		* 被冲洗后出队不会再被阻塞。
		/// </summary>
		std::atomic_bool _flushed = false;

		/// <summary>
		///		队列容量的上限。
		/// </summary>
		size_t _max = 0;

		/// <summary>
		///		队列元素数量小于此值时取消对入队的阻塞。
		/// </summary>
		size_t _threshold = 0;

		SafeQueue<T> _queue;

		/**
		* @brief 非私有方法需要加锁。
		*/
		std::mutex _not_private_methods_lock;

		/**
		* @brief 队列中元素数量小于阈值时条件成立。
		*/
		std::condition_variable _queue_consumed_cv;

		/**
		* @brief 队列中元素数量大于阈值时条件成立。
		*/
		std::condition_variable _queue_avaliable_cv;

	public:
		HysteresisBlockingQueue(size_t max)
		{
			using namespace std;
			if (max == 0)
			{
				throw std::invalid_argument{ CODE_POS_STR + std::string{"最大值不能是 0"} };
			}

			_max = max;
			_threshold = _max / 2;
		}

		~HysteresisBlockingQueue()
		{
			Dispose();
		}

		/// <summary>
		///		* 清空队列。
		///		* 取消所有阻塞。
		/// </summary>
		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;

			_queue.Clear();
			_queue_consumed_cv.notify_all();
			_queue_avaliable_cv.notify_all();
		}

		/**
		 * @brief 队列中当前元素个数
		 * @return
		*/
		uint64_t Count() const override
		{
			// _queue 线程安全，这里不需要加锁。
			return _queue.Count();
		}

		/// <summary>
		///		退队。
		///		- 队列空了再次尝试退队会受到阻塞，直到队列里的元素数量大于阈值。
		///		- 在 Dispose 或 Flush 或析构函数执行后，本方法会被无条件取消阻塞，此时如果队列为空，
		///		  会抛出异常。
		/// </summary>
		/// <returns></returns>
		T Dequeue()
		{
			std::unique_lock<std::mutex> l(_not_private_methods_lock);

			if (_disposed)
			{
				throw std::runtime_error{ "此对象已释放，不能再使用" };
			}

			_queue_avaliable_cv.wait(l, [&]()
			{
				if (_disposed || _flushed)
				{
					// 释放后和冲洗后不再阻塞
					return true;
				}

				// 等待直到队列中有元素可以退队。
				return _queue.Count() > 0;
			});

			T element = _queue.Dequeue();
			if (_queue.Count() <= _threshold)
			{
				_queue_consumed_cv.notify_all();
			}

			return element;
		}

		bool TryDequeue(T &out)
		{
			std::unique_lock<std::mutex> l(_not_private_methods_lock);

			if (_disposed)
			{
				throw std::runtime_error{ "此对象已释放，不能再使用" };
			}

			_queue_avaliable_cv.wait(l, [&]()
			{
				if (_disposed || _flushed)
				{
					// 释放后和冲洗后不再阻塞
					return true;
				}

				// 等待直到队列中有元素可以退队。
				return _queue.Count() > 0;
			});

			bool result = _queue.TryDequeue(out);
			if (result && _queue.Count() <= _threshold)
			{
				_queue_consumed_cv.notify_all();
			}

			return result;
		}

		/// <summary>
		///		入队。
		///		- 如果队列满了会受到阻塞，直到队列元素被消费到小于阈值时才取消阻塞。
		///		- Dispose 和 Flush 会无条件取消阻塞。但是，Dispose 和 Flush 后，
		///		  再次尝试入队会抛出异常。
		/// </summary>
		/// <param name="obj"></param>
		void Enqueue(T obj)
		{
			std::unique_lock<std::mutex> l(_not_private_methods_lock);

			if (_disposed)
			{
				throw std::runtime_error{ "此对象已释放，不能再使用" };
			}

			if (_flushed)
			{
				throw std::runtime_error{ "队列已冲洗，无法入队。" };
			}

			_queue_consumed_cv.wait(l, [&]()
			{
				if (_disposed || _flushed)
				{
					return true;
				}

				return _queue.Count() < _max;
			});

			_queue.Enqueue(obj);
			if (_queue.Count() >= _threshold)
			{
				_queue_avaliable_cv.notify_all();
			}
		}

		/// <summary>
		///		清空队列
		/// </summary>
		/// <exception cref="ObjectDisposedException">Dispose 后再次调用本方法会抛出异常。</exception>
		void Clear()
		{
			if (_disposed)
			{
				throw std::runtime_error{ "此对象已释放，不能再使用" };
			}

			_queue.Clear();
			_queue_consumed_cv.notify_all();
		}

		/// <summary>
		///		冲洗队列。
		///		- 冲洗后，再尝试入队会抛出异常。
		///		- 冲洗后，出队操作将不会被阻塞，即使队列中为空。
		/// </summary>
		/// <exception cref="ObjectDisposedException"></exception>
		void Flush()
		{
			if (_disposed)
			{
				throw std::runtime_error{ "此对象已释放，不能再使用" };
			}

			_flushed = true;
			_queue_consumed_cv.notify_all();
			_queue_avaliable_cv.notify_all();
		}

		/// <summary>
		///		检查队列是否已被冲洗。
		/// </summary>
		/// <returns></returns>
		bool Flushed()
		{
			return _flushed;
		}
	};
}