#pragma once
#include<jccpp/stream/Stream.h>
#include<memory>
#include<stdint.h>

class StreamHandle
{
public:
	StreamHandle(shared_ptr<Stream> stream)
	{
		*this = stream;
	}

	StreamHandle(Stream *stream)
	{
		*this = stream;
	}

public:
	StreamHandle &operator=(shared_ptr<Stream> stream)
	{
		_stream = stream;
		return *this;
	}

	StreamHandle &operator=(Stream *stream)
	{
		_stream = shared_ptr<Stream>{ stream };
		return *this;
	}

public:
	shared_ptr<Stream> _stream;
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

	int64_t Stream_Read(StreamHandle *handle, uint8_t *dst_buf, int64_t offset, int64_t count);
	void Stream_Write(StreamHandle *handle, uint8_t *src_buf, int64_t offset, int64_t count);

	void Stream_Flush(StreamHandle *handle);
	void Stream_Close(StreamHandle *handle);

	int64_t Stream_Position(StreamHandle *handle);
	void Stream_SetPosition(StreamHandle *handle, int64_t value);
}
