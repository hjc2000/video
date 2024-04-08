#pragma once
#include<algorithm>
#include<cstdint>
#include<cstring>
#include<jccpp/Exception.h>
#include<jccpp/stream/Stream.h>

namespace jc
{
	/// <summary>
	///		具有循环缓冲区的流
	/// </summary>
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
		///		队列是否已满。当头指针和尾指针重叠时，有 2 种情况：
		///			1. 缓冲区为空
		///			2. 缓冲区满
		///		此字段用来区分这两种情况。
		/// </summary>
		bool _is_full;

		void AddHead(int64_t value);
		void AddTail(int64_t value);
		int64_t ReadNonCircular(uint8_t *dst_buf, int64_t offset, int64_t count);
		void WriteNonCircular(uint8_t *src_buf, int64_t offset, int64_t count);

	public:
		/// <summary>
		///		返回内部缓冲区的大小，也是此流所能达到的最大长度。
		/// </summary>
		/// <returns></returns>
		int64_t MaxSize() const
		{
			return _buffer_size;
		}

		/// <summary>
		///		本流内部的缓冲区为空。
		/// </summary>
		/// <returns></returns>
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
		void Clear();

		#pragma region Stream
		bool CanRead() override;
		bool CanWrite() override;
		bool CanSeek() override;

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
