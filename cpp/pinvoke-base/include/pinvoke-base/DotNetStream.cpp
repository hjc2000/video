#include"DotNetStream.h"
#include<iostream>

using namespace std;

void DotNetStream::CheckError()
{
	if (_error())
	{
		/* DotNetStream 内部发生了错误。具体是什么错，可以由 C# 打印出来。
		* 反正此 DotNetStream 对象已经无法使用了。没有必要将详细的错误信息
		* 从 C# 传递到 C++。C# 的错误就在 C# 中处理。
		*/
		throw std::runtime_error{ ".NET 侧发生了错误" };
	}
}

DotNetStream::DotNetStream(
	uint8_t(*can_read)(),
	uint8_t(*can_write)(),
	uint8_t(*can_seek)(),
	int64_t(*length)(),
	void (*set_length)(int64_t value),
	int32_t(*read)(uint8_t *buffer, int32_t offset, int32_t count),
	void (*write)(uint8_t const *buffer, int32_t offset, int32_t count),
	void (*flush)(),
	void (*close)(),
	int64_t(*position)(),
	void (*set_position)(int64_t value),
	uint8_t(*error)()
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

int32_t DotNetStream::Read(uint8_t *buffer, int32_t offset, int32_t count)
{
	int32_t ret = _read(buffer, offset, count);
	CheckError();
	return ret;
}

void DotNetStream::Write(uint8_t const *buffer, int32_t offset, int32_t count)
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

DotNetStream *CreateDotNetStream(
	uint8_t(*can_read)(),
	uint8_t(*can_write)(),
	uint8_t(*can_seek)(),
	int64_t(*length)(),
	void (*set_length)(int64_t value),
	int32_t(*read)(uint8_t *buffer, int32_t offset, int32_t count),
	void (*write)(uint8_t const *buffer, int32_t offset, int32_t count),
	void (*flush)(),
	void (*close)(),
	int64_t(*position)(),
	void (*set_position)(int64_t value),
	uint8_t(*error)()
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
	};
}

void FreeDotNetStream(DotNetStream *obj)
{
	delete obj;
}
