#include<Stream.h>

int64_t Stream::ReadAtLeast(uint8_t *buffer, int64_t offset, int64_t minimum_bytes)
{
	int64_t total_read = 0;
	while (true)
	{
		int64_t have_read = Read(buffer, offset, minimum_bytes);
		if (have_read == 0)
		{
			// 到达流结尾了，读到多少就返回多少，不强求 total_read >= minimum_bytes 了。
			return total_read;
		}

		total_read += have_read;
		if (total_read >= minimum_bytes)
		{
			return total_read;
		}
	}
}

void Stream::CopyTo(Stream *dst_stream, std::shared_ptr<CancellationToken> cancellationToken)
{
	uint8_t temp_buf[1024 * 4];
	while (1)
	{
		if (cancellationToken && cancellationToken->IsCancellationRequested())
		{
			throw TaskCanceledException();
		}

		int64_t have_read = Read(temp_buf, 0, sizeof(temp_buf));
		if (have_read == 0)
		{
			return;
		}

		dst_stream->Write(temp_buf, 0, have_read);
	}
}