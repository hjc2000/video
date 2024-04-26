#include "jccpp/stream/JoinedStream.h"

/// <summary>
///		尝试从队列中获取流，如果队列为空，会触发回调然后再尝试退队。如果实在获取不到新的流，
///		本方法会返回 nullptr。
/// </summary>
/// <returns></returns>
shared_ptr<Stream> JoinedStream::TrtGetStream()
{
	if (_stream_queue.Count() == 0 && _on_current_stream_end)
	{
		_on_current_stream_end();
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

void JoinedStream::AppendStream(shared_ptr<Stream> stream)
{
	_stream_queue.Enqueue(stream);
}

bool JoinedStream::CanRead()
{
	return true;
}

bool JoinedStream::CanWrite()
{
	return false;
}

bool JoinedStream::CanSeek()
{
	return false;
}

int64_t JoinedStream::Length()
{
	throw std::runtime_error{ "不支持的操作" };
}

void JoinedStream::SetLength(int64_t value)
{
	throw std::runtime_error{ "不支持的操作" };
}

int32_t JoinedStream::Read(uint8_t *buffer, int32_t offset, int32_t count)
{
	while (1)
	{
		if (!_current_stream)
		{
			_current_stream = TrtGetStream();
			if (!_current_stream)
			{
				return 0;
			}
		}

		// 执行到这里说明 _current_stream 不为空
		int32_t have_read = _current_stream->Read(buffer, offset, count);
		if (have_read == 0)
		{
			// 此流结束了，应该尝试获取下一个流继续读取
			_current_stream = nullptr;
			continue;
		}

		_position += have_read;
		return have_read;
	}
}

void JoinedStream::Write(uint8_t const *buffer, int32_t offset, int32_t count)
{
	throw std::runtime_error{ "不支持的操作" };
}

void JoinedStream::Flush()
{
	throw std::runtime_error{ "不支持的操作" };
}

void JoinedStream::Close()
{
}

int64_t JoinedStream::Position()
{
	return _position;
}

void JoinedStream::SetPosition(int64_t value)
{
	throw std::runtime_error{ "不支持的操作" };
}
