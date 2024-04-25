#include "TsPusher.h"

bool TsPusher::CanRead()
{
	return false;
}

bool TsPusher::CanWrite()
{
	return true;
}

bool TsPusher::CanSeek()
{
	return false;
}

int64_t TsPusher::Length()
{
	throw jc::NotSupportedException();
}

void TsPusher::SetLength(int64_t value)
{
	throw jc::NotSupportedException();
}

int32_t TsPusher::Read(uint8_t *dst_buf, int32_t offset, int32_t count)
{
	throw jc::NotSupportedException();
}

void TsPusher::Flush()
{
	throw jc::NotSupportedException();
}

void TsPusher::Close()
{
	if (!_thread_exited.IsCompleted())
	{
		Stop();
	}
}

int64_t TsPusher::Position()
{
	throw jc::NotSupportedException();
}

void TsPusher::SetPosition(int64_t value)
{
	throw jc::NotSupportedException();
}
