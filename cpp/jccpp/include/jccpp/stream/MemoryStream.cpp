#include "jccpp/stream/MemoryStream.h"
#include<jccpp/Exception.h>

jc::MemoryStream::MemoryStream(int32_t max_size)
{
	if (max_size <= 0)
	{
		throw jc::ArgumentException();
	}

	_buffer_size = max_size;
	_buffer = new uint8_t[max_size];
}

bool jc::MemoryStream::CanRead()
{
	return true;
}

bool jc::MemoryStream::CanWrite()
{
	return true;
}

bool jc::MemoryStream::CanSeek()
{
	return true;
}

int64_t jc::MemoryStream::Length()
{
	return _length;
}

void jc::MemoryStream::SetLength(int64_t value)
{
	if (value > _buffer_size)
	{
		throw jc::ArgumentException();
	}

	_length = value;
	if (_position > _length)
	{
		_position = _length;
	}
}

int32_t jc::MemoryStream::Read(uint8_t *dst_buf, int32_t offset, int32_t count)
{
	if (!dst_buf)
	{
		throw jc::ArgumentNullException();
	}

	if (AvaliableToRead() == 0)
	{
		return 0;
	}

	int32_t have_read;
	if (AvaliableToRead() <= count)
	{
		have_read = AvaliableToRead();
	}
	else
	{
		have_read = count;
	}

	std::copy(
		_buffer + _position,
		_buffer + _position + have_read,
		dst_buf + offset
	);

	return have_read;
}

void jc::MemoryStream::Write(uint8_t const *src_buf, int32_t offset, int32_t count)
{
	if (!src_buf)
	{
		throw jc::ArgumentNullException();
	}

	if (count > AvaliableToWrite())
	{
		throw jc::BufferOverflowException();
	}

	std::copy(
		src_buf + offset,
		src_buf + offset + count,
		_buffer + _position
	);

	_position += count;
	if (_position > _length)
	{
		_length = _position;
	}
}

void jc::MemoryStream::Flush()
{
}

void jc::MemoryStream::Close()
{
}

int64_t jc::MemoryStream::Position()
{
	return _position;
}

void jc::MemoryStream::SetPosition(int64_t value)
{
	if (_position > _length)
	{
		throw ArgumentException();
	}

	_position = value;
}
