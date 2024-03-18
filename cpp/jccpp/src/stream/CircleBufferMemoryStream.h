#pragma once
#include<Exception.h>
#include<Stream.h>
#include<algorithm>
#include<cstdint>
#include<cstring>

namespace jc
{
	class CircleBufferMemoryStream :public Stream
	{
	public:
		CircleBufferMemoryStream(int64_t size)
		{
			_buffer_size = size;
			_head = 0;
			_tail = 0;
			_is_full = false;
			_buffer = new uint8_t[size];
		}

		~CircleBufferMemoryStream()
		{
			delete[] _buffer;
		}

	private:
		uint8_t *_buffer;
		int64_t _buffer_size;

		/**
		 * @brief 头部。读取的时候递增头部。头部指向的位置是有效元素。（前提是缓冲区不为空）
		*/
		int64_t _head;

		/**
		 * @brief 尾部。写入的时候递增尾部。尾部指向的位置是无效元素。
		 * 尾部 - 1 才是指向有效元素。（前提是缓冲区不为空）
		*/
		int64_t _tail;

		/**
		 * @brief 队列是否已满
		*/
		bool _is_full;

		/// <summary>
		///		递增头指针
		/// </summary>
		/// <param name="value"></param>
		void AddHead(int64_t value)
		{
			_head = (_head + value) % _buffer_size;
		}

		/// <summary>
		///		递增尾指针
		/// </summary>
		/// <param name="value"></param>
		void AddTail(int64_t value)
		{
			_tail = (_tail + value) % _buffer_size;
		}

		/**
		 * @brief 以非环绕方式读取。从 _head 处开始读取 count 个字节，不管会不会超出边界。
		 * 所以调用本方法前需要检查。
		 *
		 * @param dst_buf
		 * @param offset
		 * @param count
		 * @return
		*/
		int64_t ReadNonCircular(uint8_t *dst_buf, int64_t offset, int64_t count)
		{
			std::copy<uint8_t *, uint8_t *>(_buffer + _head, _buffer + _head + count, dst_buf + offset);
			AddHead(count);
			_is_full = false;
			return count;
		}

		/**
		 * @brief 以非环绕方式写入。从 _tail 的位置开始往后写入 count 个字节，不会管会不会超出边界，
		 * 所以调用本方法前需要检查。
		 *
		 * @param src_buf
		 * @param offset
		 * @param count
		*/
		void WriteNonCircular(uint8_t *src_buf, int64_t offset, int64_t count)
		{
			std::copy<uint8_t *, uint8_t *>(src_buf + offset, src_buf + offset + count, _buffer + _tail);
			AddTail(count);
			_is_full = _head == _tail;
		}

	public:
		/**
		 * @brief 返回内部缓冲区的大小，也是此流所能达到的最大长度。
		 * @return
		*/
		int64_t MaxSize() const
		{
			return _buffer_size;
		}

		/**
		 * @brief 本流内部的缓冲区为空。
		 * @return
		*/
		bool BufferEmpty() const
		{
			return _head == _tail && !_is_full;
		}

		/**
		 * @brief 本流内部的缓冲区剩余空间。单位：字节。
		 * @return
		*/
		int64_t BufferAvailableSpace() const
		{
			if (_is_full)
			{
				return 0; // 如果缓冲区已满，可用空间为0
			}
			else if (_tail >= _head)
			{
				// 如果尾指针在头指针后面，可用空间是缓冲区尾部到尾指针，加上头指针到缓冲区头部的空间
				return _buffer_size - (_tail - _head);
			}
			else
			{
				// 如果尾指针在头指针前面，可用空间是头指针到尾指针之间的空间
				return _head - _tail;
			}
		}

		virtual bool CanRead() override
		{
			return true;
		}

		virtual bool CanWrite() override
		{
			return true;
		}

		virtual bool CanSeek() override
		{
			return false;  // 循环队列通常不支持随机访问
		}

		/**
		 * @brief 流中当前有效字节数。
		 * 设计理念上，流的长度就等于流中的有效字节数，因为流中的每个字节都是有效的。
		 * （注意，不是说流缓冲区中的每个字节都是有效的。
		 *
		 * @return
		*/
		virtual int64_t Length() override
		{
			if (_is_full)
			{
				return _buffer_size;
			}

			if (_tail >= _head)
			{
				// 非满的情况下如果头尾指针重叠了，则缓冲区为空。
				return _tail - _head;
			}

			/* _head 大于 _tail。说明 _tail 穿梭了。
			*
			* 此时 _head 和 _tail 之间是空白区。举个例子，假如 _head = n, _tail = n - 1，
			* 则空白的区域大小为：
			* _head - _tail = n - (n - 1) = n - n + 1 = 1
			* 则缓冲区内容长度为：
			* _buffer_size - (_head - _tail) = _buffer_size - 1。
			*
			* 上面我们用特殊性的例子得出了具有普遍性的公式： _buffer_size - (_head - _tail)。
			*/
			return _buffer_size - (_head - _tail);
		}

		virtual void SetLength(int64_t value) override
		{
			throw jc::NotSupportedException();
		}

		virtual int64_t Read(uint8_t *dst_buf, int64_t offset, int64_t count) override
		{
			if (BufferEmpty())
			{
				return 0; // 缓冲区为空
			}

			// 应该读取多少字节
			count = std::min<int64_t>(count, Length());

			if (_tail > _head)
			{
				// 尾指针在头指针的后面，当前缓冲区内的数据没有环绕，所以读取时也不需要环绕
				return ReadNonCircular(dst_buf, offset, count);
			}

			/* 执行到这里说明 _tail <= _head，此时缓冲区内的数据发生了环绕，所以读取时有可能要环绕。*/
			if (count <= _buffer_size - _head)
			{
				// 此时从 _head 到缓冲区末尾的数据刚好够本次读取，不用环绕
				return ReadNonCircular(dst_buf, offset, count);
			}

			// 执行到这里说明要环绕了。
			// 先读取从 _head 开始到缓冲区末尾的数据
			int64_t have_read = ReadNonCircular(dst_buf, offset, _buffer_size - _head);
			count -= have_read;

			// 此时变成非环绕模式了，因为 _head 已经变成 0 了。
			have_read += ReadNonCircular(dst_buf, offset + have_read, count);
			return have_read;
		}

		virtual void Write(uint8_t *src_buf, int64_t offset, int64_t count) override
		{
			if (BufferAvailableSpace() < count)
			{
				throw jc::BufferOverflowException();
			}

			if (count <= _buffer_size - _tail)
			{
				// _tail 到缓冲区尾部的空间刚好够写入，此时不需要环绕
				WriteNonCircular(src_buf, offset, count);
				return;
			}

			// 需要环绕
			int64_t first_chunk_size = _buffer_size - _tail;
			WriteNonCircular(src_buf, offset, first_chunk_size);

			// 此时 _tail 已经变成 0 了，继续用 WriteNonCircular 写入剩余的字节
			WriteNonCircular(src_buf, offset + first_chunk_size, count - first_chunk_size);
		}

		/**
		 * @brief 调用后什么都不会发生。这是一个空的函数。
		*/
		virtual void Flush() override
		{
		}

		/**
		 * @brief 清空流。
		*/
		void Clear()
		{
			_head = 0;
			_tail = 0;
			_is_full = false;
		}

		/// <summary>
		///		没有任何作用
		/// </summary>
		virtual void Close() override
		{

		}

		/**
		 * @brief 调用将发生异常。
		 * @return
		 * @exception NotSupportedException
		*/
		virtual int64_t Position() override
		{
			throw jc::NotSupportedException();
		}

		/**
		 * @brief 调用将发生异常。
		 * @param value
		 * @exception NotSupportedException
		*/
		virtual void SetPosition(int64_t value) override
		{
			throw jc::NotSupportedException();
		}
	};

	/**
	 * @brief 测试函数
	*/
	inline void test_CircleBufferMemoryStream()
	{
		CircleBufferMemoryStream mstream{ 10 };
		uint8_t data = 0;
		while (mstream.BufferAvailableSpace())
		{
			mstream.Write(&data, 0, 1);
			data++;
		}

		// 现在流缓冲区已经满了，读取一个字节后再写入一个字节就会发生环绕。
		mstream.Read(&data, 0, 1);
		mstream.Write(&data, 0, 1);

		// 一次性读取出流中的数据
		uint8_t buf[10]{};
		mstream.Read(buf, 0, 10);
		for (int64_t i = 0; i < 10; i++)
		{
			std::cout << (int)buf[i] << std::endl;
		}
	}
}
