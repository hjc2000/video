#ifndef _STREAM_HANDLE_
#define _STREAM_HANDLE_

#include<cstdint>
#include<functional>
#include<halreg_stream.h>
#include<iostream>
#include<memory>
#include<mutex>
#include<output_modulator.h>
#include<output_modulator.h>

using std::mutex;
using std::lock_guard;
using std::shared_ptr;
using std::static_pointer_cast;
using std::cout;
using std::endl;
using std::function;

// 一个 ts 的包长度是 188 字节
#define TS_PACKET_LEN				188

// 用来读取 ts 流的缓冲区的大小
#define TSSLICE_BUFFER_LEN			CHIP_STREAM_SLICE_LEN

// 缓冲区大小除以一个 ts 包的尺寸，得出缓冲区可以储存多少个 ts 包
#define TSSLICE_PACKET_NUM			(TSSLICE_BUFFER_LEN / TS_PACKET_LEN)

#define TS_PACKET_SYNC_TAG 0x47
#define TSPACKET_PCR_PID		0x100

class TsSource
{
protected:
	/// <summary>
	///		填充缓冲区。
	/// </summary>
	/// <returns>
	///		* 成功填充会返回已填充的 ts 包的数量。可能填充 0 个包。
	///		* 发生其他错误会返回错误代码。
	/// </returns>
	virtual vk_result FillBuffer() = 0;

public:
	virtual ~TsSource() {}
	virtual vk_result Start() = 0;
	virtual vk_result Stop() = 0;
	virtual void Close() = 0;

	virtual uint32_t Bitrate() = 0;

	virtual uint8_t *TryFillAndGetBuffer() = 0;
};

#endif
