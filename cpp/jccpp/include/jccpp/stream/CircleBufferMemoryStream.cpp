#include"CircleBufferMemoryStream.h"

using namespace std;
using namespace jc;

CircleBufferMemoryStream::CircleBufferMemoryStream(int32_t size)
{
	_buffer_size = size;
	_head = 0;
	_tail = 0;
	_is_full = false;
	_buffer = new uint8_t[size];
}

CircleBufferMemoryStream::~CircleBufferMemoryStream()
{
	delete[] _buffer;
}

/// <summary>
///		递增头指针
/// </summary>
/// <param name="value"></param>
void CircleBufferMemoryStream::AddHead(int32_t value)
{
	_head = (_head + value) % _buffer_size;
}

/// <summary>
///		递增尾指针
/// </summary>
/// <param name="value"></param>
void CircleBufferMemoryStream::AddTail(int32_t value)
{
	_tail = (_tail + value) % _buffer_size;
}

/// <summary>
///		以非环绕方式读取。从 _head 处开始读取 count 个字节，不管会不会超出边界。
///		所以调用本方法前需要检查。
/// </summary>
/// <param name="buffer"></param>
/// <param name="offset"></param>
/// <param name="count"></param>
/// <returns></returns>
int32_t CircleBufferMemoryStream::ReadNonCircular(uint8_t *buffer, int32_t offset, int32_t count)
{
	std::copy(
		_buffer + _head,
		_buffer + _head + count,
		buffer + offset
	);

	AddHead(count);
	_is_full = false;
	return count;
}

/// <summary>
///		以非环绕方式写入。从 _tail 的位置开始往后写入 count 个字节，不会管会不会超出边界，
///		所以调用本方法前需要检查。
/// </summary>
/// <param name="buffer"></param>
/// <param name="offset"></param>
/// <param name="count"></param>
void CircleBufferMemoryStream::WriteNonCircular(uint8_t const *buffer, int32_t offset, int32_t count)
{
	std::copy(
		buffer + offset,
		buffer + offset + count,
		_buffer + _tail
	);

	AddTail(count);
	_is_full = _head == _tail;
}

int32_t CircleBufferMemoryStream::AvailableToWrite() const
{
	if (_is_full)
	{
		return 0; // 如果缓冲区已满，可用空间为0
	}
	else if (_tail >= _head)
	{
		// 如果尾指针在头指针后面，可用空间是缓冲区尾部到尾指针，加上头指针到缓冲区头部的空间
		return _buffer_size - (_tail - _head);
	}
	else
	{
		// 如果尾指针在头指针前面，可用空间是头指针到尾指针之间的空间
		return _head - _tail;
	}
}

void CircleBufferMemoryStream::Clear()
{
	_head = 0;
	_tail = 0;
	_is_full = false;
}

bool CircleBufferMemoryStream::CanRead()
{
	return true;
}

bool CircleBufferMemoryStream::CanWrite()
{
	return true;
}

bool CircleBufferMemoryStream::CanSeek()
{
	// 循环队列通常不支持随机访问
	return false;
}

int64_t CircleBufferMemoryStream::Length()
{
	if (_is_full)
	{
		return _buffer_size;
	}

	if (_tail >= _head)
	{
		return _tail - _head;
	}

	/* _head 大于 _tail。说明 _tail 穿梭了。
	*
	* 此时 _head 和 _tail 之间是空白区。举个例子，假如 _head = n, _tail = n - 1，
	* 则空白的区域大小为：
	* _head - _tail = n - (n - 1) = n - n + 1 = 1
	* 则缓冲区内容长度为：
	* _buffer_size - (_head - _tail) = _buffer_size - 1。
	*
	* 上面我们用特殊性的例子得出了具有普遍性的公式： _buffer_size - (_head - _tail)。
	*/
	return _buffer_size - (_head - _tail);
}

void CircleBufferMemoryStream::SetLength(int64_t value)
{
	throw std::runtime_error{ "不支持的操作" };
}

int32_t CircleBufferMemoryStream::Read(uint8_t *buffer, int32_t offset, int32_t count)
{
	if (BufferEmpty())
	{
		return 0; // 缓冲区为空
	}

	// 应该读取多少字节
	count = std::min<int64_t>(count, Length());

	if (_tail > _head)
	{
		// 尾指针在头指针的后面，当前缓冲区内的数据没有环绕，所以读取时也不需要环绕
		return ReadNonCircular(buffer, offset, count);
	}

	/* 执行到这里说明 _tail <= _head，此时缓冲区内的数据发生了环绕，所以读取时有可能要环绕。*/
	if (count <= _buffer_size - _head)
	{
		// 此时从 _head 到缓冲区末尾的数据刚好够本次读取，不用环绕
		return ReadNonCircular(buffer, offset, count);
	}

	// 执行到这里说明要环绕了。
	// 先读取从 _head 开始到缓冲区末尾的数据
	int64_t have_read = ReadNonCircular(buffer, offset, _buffer_size - _head);
	count -= have_read;

	// 此时变成非环绕模式了，因为 _head 已经变成 0 了。
	have_read += ReadNonCircular(buffer, offset + have_read, count);
	return have_read;
}

void CircleBufferMemoryStream::Write(uint8_t const *buffer, int32_t offset, int32_t count)
{
	if (AvailableToWrite() < count)
	{
		throw std::overflow_error{ "缓冲区剩余空间无法接受这么多数据" };
	}

	if (count <= _buffer_size - _tail)
	{
		// _tail 到缓冲区尾部的空间刚好够写入，此时不需要环绕
		WriteNonCircular(buffer, offset, count);
		return;
	}

	// 需要环绕
	int64_t first_chunk_size = _buffer_size - _tail;
	WriteNonCircular(buffer, offset, first_chunk_size);

	// 此时 _tail 已经变成 0 了，继续用 WriteNonCircular 写入剩余的字节
	WriteNonCircular(buffer, offset + first_chunk_size, count - first_chunk_size);
}

int64_t CircleBufferMemoryStream::Position()
{
	throw std::runtime_error{ "不支持的操作" };
}

void CircleBufferMemoryStream::SetPosition(int64_t value)
{
	throw std::runtime_error{ "不支持的操作" };
}
