#pragma once
#include<condition_variable>
#include<jccpp/IDisposable.h>
#include<jccpp/stream/CircleBufferMemoryStream.h>
#include<mutex>

namespace jc
{
	/// <summary>
	///		带阻塞功能的环形缓冲区的内存流。
	/// </summary>
	class BlockingCircleBufferMemoryStream :public Stream
	{
	public:
		BlockingCircleBufferMemoryStream(int32_t max_size) :
			_mstream(max_size)
		{

		}

		~BlockingCircleBufferMemoryStream()
		{
			Close();
		}

	private:
		CircleBufferMemoryStream _mstream;
		std::mutex _lock;
		std::atomic_bool _stream_closed = false;

		/// <summary>
		///		流中的数据被消费了，现在处于不是满的状态
		/// </summary>
		std::condition_variable _buffer_consumed;

		/// <summary>
		///		流中有数据可用。
		/// </summary>
		std::condition_variable _buffer_avaliable;

	public:
		#pragma region 通过 Stream 继承
		bool CanRead() override;
		bool CanWrite() override;
		bool CanSeek() override;
		int64_t Length() override;
		void SetLength(int64_t value) override;
		int32_t Read(uint8_t *dst_buf, int32_t offset, int32_t count) override;
		void Write(uint8_t const *src_buf, int32_t offset, int32_t count) override;
		void Flush() override;

		/// <summary>
		///		结束流。
		///		- 结束后，写入的数据会被丢弃。Read 方法在读取完缓冲区的数据后，将永远返回 0.
		///		- 结束后，会取消所有阻塞，且不会再阻塞。
		/// </summary>
		void Close() override;

		int64_t Position() override;
		void SetPosition(int64_t value) override;
		#pragma endregion

	};
}