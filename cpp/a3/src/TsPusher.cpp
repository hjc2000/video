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
	throw std::runtime_error{ "不支持的操作" };
}

void TsPusher::SetLength(int64_t value)
{
	throw std::runtime_error{ "不支持的操作" };
}

int32_t TsPusher::Read(uint8_t *dst_buf, int32_t offset, int32_t count)
{
	throw std::runtime_error{ "不支持的操作" };
}

void TsPusher::Flush()
{
	throw std::runtime_error{ "不支持的操作" };
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
	throw std::runtime_error{ "不支持的操作" };
}

void TsPusher::SetPosition(int64_t value)
{
	throw std::runtime_error{ "不支持的操作" };
}
