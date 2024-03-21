#pragma once
#include<AVError.h>
#include<Stream.h>
#include<Wrapper.h>
#include<include_ffmpeg.h>

namespace video
{
	class AVIOContextWrapper :public Wrapper<AVIOContext>
	{
	public:
		AVIOContextWrapper(bool is_write, shared_ptr<Stream> stream);
		~AVIOContextWrapper();

	private:
		int _buffer_size;
		uint8_t *_buffer;
		shared_ptr<Stream> _stream;

	private:
		static int StaticReadPacket(void *opaque, uint8_t *buf, int buf_size);
		static int StaticWritePacket(void *opaque, uint8_t const *buf, int buf_size);
		static int64_t StaticSeek(void *opaque, int64_t offset, int whence);

		int ReadPacket(uint8_t *buf, int buf_size);
		int WritePacket(uint8_t *buf, int buf_size);
		int64_t Seek(int64_t offset, int whence);
	};
}