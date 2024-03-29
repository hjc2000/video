#include "MemoryStream.h"
#include<Exception.h>

jc::MemoryStream::MemoryStream(int64_t max_size)
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

int64_t jc::MemoryStream::Read(uint8_t *dst_buf, int64_t offset, int64_t count)
{
	if (!dst_buf)
	{
		throw jc::ArgumentNullException();
	}

	if (AvaliableToRead() == 0)
	{
		return 0;
	}

	int64_t have_read;
	if (AvaliableToRead() <= count)
	{
		have_read = AvaliableToRead();
	}
	else
	{
		have_read = count;
	}

	std::copy<uint8_t *, uint8_t *>(_buffer + _position, _buffer + _position + have_read, dst_buf + offset);
	return have_read;
}

void jc::MemoryStream::Write(uint8_t *src_buf, int64_t offset, int64_t count)
{
	if (!src_buf)
	{
		throw jc::ArgumentNullException();
	}

	if (count > AvaliableToWrite())
	{
		throw jc::BufferOverflowException();
	}

	std::copy<uint8_t *, uint8_t *>(src_buf + offset, src_buf + offset + count, _buffer + _position);
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
