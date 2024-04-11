#include"DotNetStream.h"

DotNetStream::DotNetStream(
	bool (*can_read)(),
	bool (*can_write)(),
	bool (*can_seek)(),
	int64_t(*length)(),
	void (*set_length)(int64_t value),
	int64_t(*read)(uint8_t *buffer, int64_t offset, int64_t count),
	void (*write)(uint8_t *buffer, int64_t offset, int64_t count),
	void (*flush)(),
	void (*close)(),
	int64_t(*position)(),
	void (*set_position)(int64_t value),
	bool (*error)(),
	int64_t(*error_message_size)(),
	uint8_t *(*error_message_buffer)()
)
{
	_can_read = can_read;
	_can_write = can_write;
	_can_seek = can_seek;
	_length = length;
	_set_length = set_length;
	_read = read;
	_write = write;
	_flush = flush;
	_close = close;
	_position = position;
	_set_position = set_position;
	_error = error;
	_error_message_size = error_message_size;
	_error_message_buffer = error_message_buffer;
}

void DotNetStream::CheckError()
{
	if (_error())
	{
		std::string error_message{
			(char const *)_error_message_buffer(),
			(size_t)_error_message_size()
		};

		throw jc::Exception(error_message);
	}
}

bool DotNetStream::CanRead()
{
	bool ret = _can_read();
	CheckError();
	return ret;
}

bool DotNetStream::CanWrite()
{
	bool ret = _can_write();
	CheckError();
	return ret;
}

bool DotNetStream::CanSeek()
{
	bool ret = _can_seek();
	CheckError();
	return ret;
}

int64_t DotNetStream::Length()
{
	int64_t ret = _length();
	CheckError();
	return ret;
}

void DotNetStream::SetLength(int64_t value)
{
	_set_length(value);
	CheckError();
}

int64_t DotNetStream::Read(uint8_t *buffer, int64_t offset, int64_t count)
{
	int64_t ret = _read(buffer, offset, count);
	CheckError();
	return ret;
}

void DotNetStream::Write(uint8_t *buffer, int64_t offset, int64_t count)
{
	_write(buffer, offset, count);
	CheckError();
}

void DotNetStream::Flush()
{
	_flush();
	CheckError();
}

void DotNetStream::Close()
{
	_close();
	CheckError();
}

int64_t DotNetStream::Position()
{
	int64_t ret = _position();
	CheckError();
	return ret;
}

void DotNetStream::SetPosition(int64_t value)
{
	_set_position(value);
	CheckError();
}

DotNetStream *CreateDotnetStream(
	bool (*can_read)(),
	bool (*can_write)(),
	bool (*can_seek)(),
	int64_t(*length)(),
	void (*set_length)(int64_t value),
	int64_t(*read)(uint8_t *buffer, int64_t offset, int64_t count),
	void (*write)(uint8_t *buffer, int64_t offset, int64_t count),
	void (*flush)(),
	void (*close)(),
	int64_t(*position)(),
	void (*set_position)(int64_t value),
	bool (*error)(),
	int64_t(*error_message_size)(),
	uint8_t *(*error_message_buffer)()
)
{
	return new DotNetStream{
		can_read,
		can_write,
		can_seek,
		length,
		set_length,
		read,
		write,
		flush,
		close,
		position,
		set_position,
		error,
		error_message_size,
		error_message_buffer,
	};
}

void FreeDotnetStream(DotNetStream *obj)
{
	delete obj;
}
