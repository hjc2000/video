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
		CircleBufferMemoryStream(int64_t size);
		~CircleBufferMemoryStream();

	private:
		uint8_t *_buffer;
		int64_t _buffer_size;

		/// <summary>
		///		头部。读取的时候递增头部。头部指向的位置是有效元素。（前提是缓冲区不为空）
		/// </summary>
		int64_t _head;

		/// <summary>
		///		尾部。写入的时候递增尾部。尾部指向的位置是无效元素。
		///		尾部 - 1 才是指向有效元素。（前提是缓冲区不为空）
		/// </summary>
		int64_t _tail;

		/// <summary>
		///		队列是否已满。当头指针和尾指针重叠时，有 2 种情况。
		///		1. 缓冲区为空
		///		2. 缓冲区满
		///		此字段用来区分这两种情况。
		/// </summary>
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

		int64_t ReadNonCircular(uint8_t *dst_buf, int64_t offset, int64_t count);
		void WriteNonCircular(uint8_t *src_buf, int64_t offset, int64_t count);

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

		/// <summary>
		///		本流内部的缓冲区剩余空间。单位：字节。
		/// </summary>
		/// <returns></returns>
		int64_t BufferAvailableSpace() const;

		/// <summary>
		///		清空流
		/// </summary>
		void Clear()
		{
			_head = 0;
			_tail = 0;
			_is_full = false;
		}

		#pragma region Stream
		bool CanRead() override
		{
			return true;
		}

		bool CanWrite() override
		{
			return true;
		}

		bool CanSeek() override
		{
			return false;  // 循环队列通常不支持随机访问
		}

		/// <summary>
		///		流中当前剩余的有效的，可读的字节数。
		/// </summary>
		/// <returns></returns>
		int64_t Length() override;

		/// <summary>
		///		不支持的操作。调用将引发异常。
		/// </summary>
		/// <param name="value"></param>
		void SetLength(int64_t value) override;

		int64_t Read(uint8_t *dst_buf, int64_t offset, int64_t count) override;
		void Write(uint8_t *src_buf, int64_t offset, int64_t count) override;

		/// <summary>
		///		调用后什么都不会发生。这是一个空的函数。
		/// </summary>
		void Flush() override
		{
		}

		/// <summary>
		///		没有任何作用
		/// </summary>
		void Close() override
		{

		}

		/// <summary>
		///		调用将发生 NotSupportedException
		/// </summary>
		/// <returns></returns>
		int64_t Position() override;

		/// <summary>
		///		调用将发生 NotSupportedException
		/// </summary>
		/// <param name="value"></param>
		void SetPosition(int64_t value) override;
		#pragma endregion

	};
}
