#pragma once
#include<jccpp/Exception.h>
#include<jccpp/IDisposable.h>
#include<jccpp/container/IQueue.h>
#include<jccpp/container/SafeQueue.h>
#include<condition_variable>
#include<jccpp/define.h>
#include<iostream>
#include<mutex>

namespace jc
{
	/**
	 * @brief 带有滞回特性的阻塞队列。
	 * - 队列满时，再往里送会阻塞，直到队列中的元素被消费到小于一定阈值才取消阻塞。
	 * - 队列空时，再往外拿会阻塞，直到队列中的元素大于一定阈值才取消阻塞。
	 * - 本队列线程安全。
	 *
	 * @tparam T
	*/
	template<typename T>
	class HysteresisBlockingQueue :public IQueue<T>, public IDisposable
	{
		#pragma region 生命周期
	public:
		HysteresisBlockingQueue(size_t max)
		{
			using namespace std;
			if (max == 0)
			{
				cout << CODE_POS_STR << "最大值不能是 0" << endl;
				throw jc::ArgumentException();
			}

			_max = max;
			_threshold = _max / 2;
		}

		~HysteresisBlockingQueue()
		{
			Dispose();
		}

	private:
		std::atomic_bool _disposed = false;

	public:
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
		#pragma endregion

	private:
		/// <summary>
		///		* 被冲洗后入队会抛出异常。
		///		* 被冲洗后，_queue_avaliable 将被 Dispose，出队不会再被阻塞。
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
		/**
		 * @brief 队列中当前元素个数
		 * @return
		*/
		uint64_t Count() override
		{
			// _queue 线程安全，这里不需要加锁。
			return _queue.Count();
		}

		/**
		 * @brief 退队。
		 * 注意，在 Dispose 或析构函数执行后，本方法会被无条件取消阻塞，此时如果队列为空，
		 * 会抛出异常。
		 *
		 * @return
		 * @exception InvalidOperationException 队列为空会引发此异常。
		 * @exception ObjectDisposedException
		*/
		T Dequeue()
		{
			std::unique_lock<std::mutex> l(_not_private_methods_lock);

			if (_disposed)
			{
				throw jc::ObjectDisposedException();
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

		/**
		 * @brief
		 * @param out
		 * @return
		 * @exception ObjectDisposedException
		*/
		bool TryDequeue(T &out)
		{
			std::unique_lock<std::mutex> l(_not_private_methods_lock);

			if (_disposed)
			{
				throw jc::ObjectDisposedException();
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

		/**
		 * @brief 入队。
		 * - 如果队列满了会受到阻塞，直到队列元素被消费到小于阈值时才取消阻塞。
		 *
		 * @param obj 要入队的对象。
		 * @exception InvalidOperationException 冲洗后如果再调用本方法会抛出异常。
		 * @exception ObjectDisposedException Dispose 后再次调用本方法会抛出异常。
		*/
		void Enqueue(T obj)
		{
			std::unique_lock<std::mutex> l(_not_private_methods_lock);

			if (_disposed)
			{
				throw jc::ObjectDisposedException();
			}

			if (_flushed)
			{
				throw jc::InvalidOperationException();
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
				throw jc::ObjectDisposedException();
			}

			_queue.Clear();
			_queue_consumed_cv.notify_all();
		}

		/// <summary>
		///		冲洗队列。
		///		* 冲洗后，再尝试入队会抛出异常。
		///		* 冲洗后，出队操作将不会被阻塞，即使队列中为空。
		/// </summary>
		/// <exception cref="ObjectDisposedException"></exception>
		void Flush()
		{
			if (_disposed)
			{
				throw jc::ObjectDisposedException();
			}

			_flushed = true;
			_queue_consumed_cv.notify_all();
			_queue_avaliable_cv.notify_all();
		}

		bool Flushed()
		{
			return _flushed;
		}
	};
}