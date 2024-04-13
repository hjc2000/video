#include "BlockingCircleBufferMemoryStream.h"

bool jc::BlockingCircleBufferMemoryStream::CanRead()
{
	return true;
}

bool jc::BlockingCircleBufferMemoryStream::CanWrite()
{
	return true;
}

bool jc::BlockingCircleBufferMemoryStream::CanSeek()
{
	return false;
}

int64_t jc::BlockingCircleBufferMemoryStream::Length()
{
	std::unique_lock l(_lock);
	return _mstream.Length();
}

void jc::BlockingCircleBufferMemoryStream::SetLength(int64_t value)
{
	std::unique_lock l(_lock);
	_mstream.SetLength(value);
}

int64_t jc::BlockingCircleBufferMemoryStream::Read(uint8_t *dst_buf, int64_t offset, int64_t count)
{
	std::unique_lock l(_lock);

	// 等待流中有数据可用
	_buffer_avaliable.wait(l, [&]()->bool
	{
		if (_stream_closed)
		{
			return true;
		}

		return _mstream.Length();
	});

	int64_t have_read = _mstream.Read(dst_buf, offset, count);
	_buffer_consumed.notify_all();
	return have_read;
}

void jc::BlockingCircleBufferMemoryStream::Write(uint8_t *src_buf, int64_t offset, int64_t count)
{
	std::unique_lock l(_lock);

	while (count > 0)
	{
		if (_stream_closed)
		{
			return;
		}

		// 等待流缓冲区中有空间可写
		_buffer_consumed.wait(l, [&]()->bool
		{
			if (_stream_closed)
			{
				return true;
			}

			return _mstream.BufferAvailableSpace();
		});

		int64_t should_write = std::min(_mstream.BufferAvailableSpace(), count);
		_mstream.Write(src_buf, offset, should_write);
		offset += should_write;
		count -= should_write;
		_buffer_avaliable.notify_all();
	}
}

void jc::BlockingCircleBufferMemoryStream::Flush()
{
	std::unique_lock l(_lock);
	_mstream.Flush();
}

void jc::BlockingCircleBufferMemoryStream::Close()
{
	std::unique_lock l(_lock);
	_stream_closed = true;
	_buffer_avaliable.notify_all();
	_buffer_consumed.notify_all();
}

int64_t jc::BlockingCircleBufferMemoryStream::Position()
{
	std::unique_lock l(_lock);
	return _mstream.Position();
}

void jc::BlockingCircleBufferMemoryStream::SetPosition(int64_t value)
{
	std::unique_lock l(_lock);
	_mstream.SetPosition(value);
}
