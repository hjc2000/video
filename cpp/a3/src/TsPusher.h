#pragma once
#include<A3Device.h>
#include<jccpp/TaskCompletionSignal.h>
#include<jccpp/stream/BlockingCircleBufferMemoryStream.h>

class TsPusher :public Stream
{
public:
	TsPusher(shared_ptr<A3Device> device)
	{
		_device = device;
	}

	~TsPusher()
	{
		Close();
	}

private:
	shared_ptr<A3Device> _device;
	jc::BlockingCircleBufferMemoryStream _mstream{ CHIP_STREAM_SLICE_LEN * 2 };
	TaskCompletionSignal _thread_exited{ true };
	std::atomic_bool _should_exit = false;

	/**
	 * @brief 向 A3 发送 ts 数据块的线程函数。
	*/
	void send_block_thread_func()
	{
		unique_ptr<uint8_t[]> slice_buffer{ new uint8_t[CHIP_STREAM_SLICE_LEN] };

		while (!_should_exit)
		{
			// A3 缓冲区中剩余的空间可以容纳的包数量。
			uint32_t a3_avaliable_packet_space = _device->A3AvailablePacketSpace();
			if (a3_avaliable_packet_space == 0)
			{
				// A3 缓冲区满
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			while (!_should_exit)
			{
				if (a3_avaliable_packet_space < CHIP_STREAM_SLICE_LEN / CHIP_TS_PACKET_LEN)
				{
					_device->CommitPackets();
					break;
				}

				int64_t have_read = 0;
				while (have_read < CHIP_STREAM_SLICE_LEN && !_should_exit)
				{
					have_read += _mstream.Read(slice_buffer.get(), have_read, CHIP_STREAM_SLICE_LEN - have_read);
				}

				_device->WriteBlock(slice_buffer.get(), CHIP_STREAM_SLICE_LEN);
				a3_avaliable_packet_space -= CHIP_STREAM_SLICE_LEN / CHIP_TS_PACKET_LEN;
			}
		}

		_thread_exited.SetResult();
	}

	vk_result Stop()
	{
		_should_exit = true;
		_mstream.Close();

		// 等待线程函数返回
		_thread_exited.Wait();

		vk_result nres = _device->StopStream();
		if (!is_vatek_success(nres))
		{
			std::cout << "stop device stream fail" << std::endl;
		}

		nres = _device->vatek_transform_stop();
		return nres;
	}

public:
	void Start()
	{
		_should_exit = false;
		_thread_exited.Reset();
		thread([&]()
		{
			send_block_thread_func();
		}).detach();
	}

	#pragma region 通过 Stream 继承
	void Write(uint8_t const *src_buf, int32_t offset, int32_t count) override
	{
		_mstream.Write(src_buf, offset, count);
	}

	bool CanRead() override;
	bool CanWrite() override;
	bool CanSeek() override;
	int64_t Length() override;
	void SetLength(int64_t value) override;
	int32_t Read(uint8_t *dst_buf, int32_t offset, int32_t count) override;
	void Flush() override;
	void Close() override;
	int64_t Position() override;
	void SetPosition(int64_t value) override;
	#pragma endregion

};