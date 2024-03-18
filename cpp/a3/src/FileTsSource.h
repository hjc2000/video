#pragma once
#include<MemoryStream.h>
#include<TsSource.h>

class FileTsSource :public TsSource
{
private:
	/// <summary>
	///		将文件指针移动到以文件开始为参考点的 amount_of_packets_read + offset 处，然后读取 1 个字节，检查是否是 ts
	///		的同步字节。
	/// </summary>
	/// <param name="hfile"></param>
	/// <param name="amount_of_packets_read"></param>
	/// <param name="offset"></param>
	/// <returns>
	///		是同步字节返回 1，不是同步字节或发生了错误则返回错误代码。
	///		如果是格式错误，返回的错误代码为 vatek_format。
	/// </returns>
	vk_result file_check_sync(int32_t pos, int32_t offset)
	{
		// seek 成功返回 0，失败返回 -1
		vk_result nres = (vk_result)fseek(fhandle, pos + offset, SEEK_SET);
		if (nres < 0)
		{
			// seek 失败，返回 -1
			return (vk_result)-1;
		}

		uint8_t tag = 0;

		// 读取 1 个字节
		nres = (vk_result)fread(&tag, 1, 1, fhandle);
		if (nres != 1)
		{
			// 没有成功读取到 1 个字节
			return vatek_hwfail;
		}

		// 检查读取到的 1 个字节是否是 ts 的同步字节
		if (tag == TS_PACKET_SYNC_TAG)
		{
			// 是同步字节
			return (vk_result)1;
		}

		// 不是同步字节，返回格式错误的错误代码
		return vatek_format;
	}

	vk_result file_lock()
	{
		vk_result nres = vatek_badstatus;
		uint8_t sync;
		uint32_t count = 0;

		for (;;)
		{
			// 获取当前文件指针
			size_t pos = ftell(fhandle);

			/* 读取 1 个字节，返回值为成功读取的元素个数，而不是字节数。在这里，一个元素就是 1 个字节，所以元素数
			* 等于字节数。
			*/
			nres = (vk_result)fread(&sync, 1, 1, fhandle);
			if (nres != 1)
			{
				// 不等于 1，说明没有成功读取到 1 个字节，返回错误代码
				return vatek_hwfail;
			}

			// 成功读取到 1 个字节，检查读取到的 1 个字节是否是 ts 的同步字节
			if (sync == TS_PACKET_SYNC_TAG)
			{
				// 如果是同步字节
				packet_len = 0;

				/* amount_of_packets_read 的值是一进入循环就第一时间获取了，此时还没有读取任何一个字节。执行到这里说明读取了 1 个
				* 字节，并且这个字节是同步字节。
				*
				* 调用 file_check_sync 函数后，内部会以文件开头为参考点，seek 一段 amount_of_packets_read + TS_PACKET_LEN 的距离，
				* 因为 amount_of_packets_read 指向的是同步字节的位置，而 TS_PACKET_LEN 等于一个 ts 包的长度，
				* 所以 seek 后文件指针又是处于同步字节的位置。然后 file_check_sync 会读取 1 个字节并检查是否是
				* 同步字节。如果是同步字节，加上刚才读取的同步字节，总共同步到 2 个包。
				*/
				nres = file_check_sync((int32_t)pos, TS_PACKET_LEN);
				if (is_vatek_success(nres))
				{
					// 是同步字节
					packet_len = TS_PACKET_LEN;
				}
				else
				{
					// 不是同步字节，可能是因为此 ts 流的包大小不是 188 字节，而是 204 字节，再试一次。
					nres = file_check_sync((int32_t)pos, 204);

					if (is_vatek_success(nres))
					{
						// 再试一次后如果是同步字节
						packet_len = 204;
					}
				}

				if (packet_len != 0)
				{
					// pfile->packet_len != 0 说明前面的 file_check_sync 成功了
					// 成功后 seek 回原来的位置。将 seek 的结果作为返回值。成功为 0，失败为 -1.
					nres = (vk_result)fseek(fhandle, (int32_t)pos, SEEK_SET);
					return nres;
				}
			}

			// 没有读取到第 1 个同步字节就递增计数，等下一次循环再次读取下一个字节
			count++;
			// 计数溢出后还没锁定到 ts 流，就超时
			if (count > 1000)
				return vatek_timeout;
		}
	}

public:
	int32_t packet_len{};
	int32_t file_size{};
	FILE *fhandle{};
	uint8_t _buffer[TSSLICE_BUFFER_LEN]{};
	uint32_t _amout_of_total_packets_read{};
	int64_t tick{};
	jc::MemoryStream _mstream{ TSSLICE_BUFFER_LEN };
	uint8_t _one_packet_buffer[300]{};

public:
	static vk_result cross_stream_file_get(const char *filename, shared_ptr<FileTsSource> pcstream);

	vk_result Start() override;
	vk_result Stop() override;

	vk_result FillBuffer()
	{
		vk_result nres = vatek_success;

		/* TSSLICE_PACKET_NUM 是用缓冲区大小除以 188，即缓冲区能容纳多少个 ts 包。
		 * 在循环中将数据以一个包为单位，从文件中读取出来然后写入缓冲区。
		*/
		while (_mstream.Length() < TSSLICE_BUFFER_LEN)
		{
			int element_read = (int)fread(_one_packet_buffer, packet_len, 1, fhandle);
			if (element_read != 1)
			{
				/* 读取失败，就 seek 回文件头，然后锁定 ts 流，然后 continue。
				 * 为什么要这样？可能是为了处理因为即将到达文件尾所以读取包失败的情况。
				*/
				fseek(fhandle, 0, SEEK_SET);
				nres = file_lock();
				if (is_vatek_success(nres))
				{
					continue;
				}
			}

			// 读取包成功
			// 检查包的第一个字节是否是同步字节
			if (_one_packet_buffer[0] == TS_PACKET_SYNC_TAG)
			{
				// 是同步字节
				_mstream.Write(_one_packet_buffer, 0, packet_len);
			}
			else
			{
				nres = file_lock();
				if (!is_vatek_success(nres))
					break;
			}
		}

		_mstream.SetPosition(0);
		_mstream.Read(_buffer, 0, TSSLICE_BUFFER_LEN);
		_mstream.SetLength(0);

		if (is_vatek_success(nres))
		{
			nres = (vk_result)1;
		}

		return nres;
	}

	uint8_t *TryFillAndGetBuffer()
	{
		if (FillBuffer() > 0)
		{
			// 成功填充了 0 个以上的包才返回缓冲区的指针。
			_amout_of_total_packets_read += TSSLICE_PACKET_NUM;
			return _buffer;
		}

		return nullptr;
	}

	uint32_t Bitrate() override;
	void Close() override;
};
