#include "JoinedStream.h"

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
	throw jc::NotSupportedException();
}

void JoinedStream::SetLength(int64_t value)
{
	throw jc::NotSupportedException();
}

int64_t JoinedStream::Read(uint8_t *dst_buf, int64_t offset, int64_t count)
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
		int64_t have_read = _current_stream->Read(dst_buf, offset, count);
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

void JoinedStream::Write(uint8_t *src_buf, int64_t offset, int64_t count)
{
	throw jc::NotSupportedException();
}

void JoinedStream::Flush()
{
	throw jc::NotSupportedException();
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
	throw jc::NotSupportedException();
}
