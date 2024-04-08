#pragma once
#include<functional>
#include<jccpp/Exception.h>
#include<jccpp/container/SafeQueue.h>
#include<jccpp/stream/Stream.h>
#include<memory>

using std::shared_ptr;

class JoinedStream :public Stream
{
private:
	/// <summary>
	///		用来统计总共从 Read 函数中读取过多少字节。
	/// </summary>
	int64_t _position = 0;
	SafeQueue<shared_ptr<Stream>> _stream_queue{};
	shared_ptr<Stream> _current_stream;

	shared_ptr<Stream> TrtGetStream();

public:
	/// <summary>
	///		当前流读到尽头时就会触发此回调。需要调用 AppendStream 方法添加流到本对象，
	///		否则 JoinedStream 将结束。
	/// </summary>
	std::function<void()> _stream_end_callback;

	/// <summary>
	///		向本对象中追加流。如果不追加，在队列中所有流都读完后，JoinedStream 也将结束。
	/// </summary>
	/// <param name="stream"></param>
	void AppendStream(shared_ptr<Stream> stream);

	#pragma region Stream
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
	#pragma endregion
};