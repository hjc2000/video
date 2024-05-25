#pragma once
#include<base/Stream.h>
#include<memory>
#include<stdint.h>

/// <summary>
///		因为 C++ 这边的 Stream 一般都是用 shared_ptr 来管理的，所以需要一个 StreamHandle
///		类来包装 shared_ptr，否则将堆上的 shared_ptr 对象的指针 shared_ptr* 传给 C# 会
///		造成逻辑混乱。shared_ptr 意义就是指针了，又搞出一个 shared_ptr*，所以就定义了本类。
/// </summary>
class StreamHandle
{
public:
	StreamHandle(std::shared_ptr<base::Stream> stream)
	{
		*this = stream;
	}

	StreamHandle(base::Stream *stream)
	{
		*this = stream;
	}

public:
	StreamHandle &operator=(std::shared_ptr<base::Stream> stream)
	{
		_stream = stream;
		return *this;
	}

	StreamHandle &operator=(base::Stream *stream)
	{
		_stream = std::shared_ptr<base::Stream> { stream };
		return *this;
	}

public:
	std::shared_ptr<base::Stream> _stream;
};


extern "C"
{
	StreamHandle *GetFileStream(std::string *url);
	void FreeStream(StreamHandle *handle);

	uint8_t Stream_CanRead(StreamHandle *handle);
	uint8_t Stream_CanWrite(StreamHandle *handle);
	uint8_t Stream_CanSeek(StreamHandle *handle);

	int64_t Stream_Length(StreamHandle *handle);
	void Stream_SetLength(StreamHandle *handle, int64_t value);

	int32_t Stream_Read(StreamHandle *handle, uint8_t *dst_buf, int32_t offset, int32_t count);
	void Stream_Write(StreamHandle *handle, uint8_t *src_buf, int32_t offset, int32_t count);

	void Stream_Flush(StreamHandle *handle);
	void Stream_Close(StreamHandle *handle);

	int64_t Stream_Position(StreamHandle *handle);
	void Stream_SetPosition(StreamHandle *handle, int64_t value);
}
