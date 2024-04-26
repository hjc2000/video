#include "jccpp/stream/MemoryStream.h"
#include<jccpp/Exception.h>

jc::MemoryStream::MemoryStream(int32_t max_size)
{
	if (max_size <= 0)
	{
		throw std::invalid_argument{ "max_size 不能小于等于 0。" };
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
		throw std::invalid_argument{ "value 不能大于缓冲区大小。" };
	}

	_length = value;
	if (_position > _length)
	{
		_position = _length;
	}
}

int32_t jc::MemoryStream::Read(uint8_t *buffer, int32_t offset, int32_t count)
{
	if (!buffer)
	{
		throw std::invalid_argument{ "buffer 不能是空指针" };
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
		buffer + offset
	);

	return have_read;
}

void jc::MemoryStream::Write(uint8_t const *buffer, int32_t offset, int32_t count)
{
	if (!buffer)
	{
		throw std::invalid_argument{ "buffer 不能是空指针" };
	}

	if (count > AvaliableToWrite())
	{
		throw std::overflow_error{ "缓冲区剩余空间无法接受这么多数据" };
	}

	std::copy(
		buffer + offset,
		buffer + offset + count,
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
		throw std::invalid_argument{ "value 不能流的长度。" };
	}

	_position = value;
}
