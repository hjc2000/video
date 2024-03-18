#pragma once
#include<Exception.h>
#include<SafeQueue.h>
#include<Stream.h>
#include<functional>
#include<memory>

using std::shared_ptr;

class JoinedStream :public Stream
{
public:

private:
	/**
	 * @brief 用来统计总共从 Read 函数中读取过多少字节。
	*/
	int64_t _position = 0;
	SafeQueue<shared_ptr<Stream>> _stream_queue{};
	shared_ptr<Stream> _current_stream;

	/**
	 * @brief 尝试从队列中获取流，如果队列为空，会触发回调然后再尝试退队。如果实在获取不到新的流，
	 * 本方法会返回 nullptr。
	 *
	 * @return
	*/
	shared_ptr<Stream> TrtGetStream()
	{
		if (_stream_queue.Count() == 0 && _stream_end_callback)
		{
			_stream_end_callback();
		}

		try
		{
			return _stream_queue.Dequeue();
		}
		catch (...)
		{
			return nullptr;
		}
	}

public:
	/**
	 * @brief 当前流读到尽头时就会触发此回调。需要调用 AppendStream 方法添加流到本对象，
	 * 否则 JoinedStream 将结束。
	*/
	std::function<void()> _stream_end_callback;

	/**
	 * @brief 向本对象中追加流。如果不追加，在队列中所有流都读完后，JoinedStream 也将结束。
	 * @param stream
	*/
	void AppendStream(shared_ptr<Stream> stream)
	{
		_stream_queue.Enqueue(stream);
	}

	bool CanRead() override;

	bool CanWrite() override;

	bool CanSeek() override;

	int64_t Length() override;

	void SetLength(int64_t value) override;

	int64_t Read(uint8_t *dst_buf, int64_t offset, int64_t count) override;

	void Write(uint8_t *src_buf, int64_t offset, int64_t count) override;

	void Flush() override;

	void Close() override;

	int64_t Position() override;

	void SetPosition(int64_t value) override;

};