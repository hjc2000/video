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
		AVIOContextWrapper(bool is_write, shared_ptr<Stream> stream) :
			_stream(stream)
		{
			_buffer_size = 1024 * 64;
			_buffer = (uint8_t *)av_malloc(_buffer_size);

			_wrapped_obj = avio_alloc_context(
				_buffer,
				_buffer_size,
				is_write,
				this,
				StaticReadPacket,
				StaticWritePacket,
				StaticSeek
			);
		}

		~AVIOContextWrapper()
		{
			// av_free(_buffer);
			/* 不要执行：av_free(_buffer);
			*
			* 将 _buffer 传给 avio_alloc_context 函数后，创建出来的 AVIOContext 就拥有缓冲区所有权，
			* 调用 avformat_close_input(&_wrapped_obj); 或 avformat_free_context(_wrapped_obj);
			* 时会释放这个缓冲区。所以这里如果 av_free(_buffer); 会重复释放。
			*/

			_stream->Close();
		}

	private:
		int _buffer_size;
		uint8_t *_buffer;
		shared_ptr<Stream> _stream;

	private:
		#pragma region 静态接口函数
		static int StaticReadPacket(void *opaque, uint8_t *buf, int buf_size)
		{
			AVIOContextWrapper *me = (AVIOContextWrapper *)opaque;
			return me->ReadPacket(buf, buf_size);
		}

		static int StaticWritePacket(void *opaque, uint8_t const *buf, int buf_size)
		{
			AVIOContextWrapper *me = (AVIOContextWrapper *)opaque;
			return me->WritePacket((uint8_t *)buf, buf_size);
		}

		static int64_t StaticSeek(void *opaque, int64_t offset, int whence)
		{
			AVIOContextWrapper *me = (AVIOContextWrapper *)opaque;
			return me->Seek(offset, whence);
		}
		#pragma endregion

		#pragma region 实例接口函数
		int ReadPacket(uint8_t *buf, int buf_size)
		{
			try
			{
				int have_read = (int)_stream->Read(buf, 0, buf_size);
				if (have_read == 0)
				{
					return (int)ErrorCode::eof;
				}

				return have_read;
			}
			catch (...)
			{
				return -1;
			}
		}

		int WritePacket(uint8_t *buf, int buf_size)
		{
			try
			{
				_stream->Write(buf, 0, buf_size);
				return 0;
			}
			catch (...)
			{
				return -1;
			}
		}

		int64_t Seek(int64_t offset, int whence)
		{
			if (!_stream->CanSeek())
			{
				return -1;
			}

			switch (whence)
			{
			case SEEK_SET:
				{
					_stream->SetPosition(offset);
					return _stream->Position();
				}
			case SEEK_CUR:
				{
					size_t current_pos = _stream->Position();
					_stream->SetPosition(current_pos + offset);
					return _stream->Position();
				}
			case SEEK_END:
				{
					size_t end_pos = _stream->Length();
					_stream->SetPosition(end_pos + offset);
					return _stream->Position();
				}
			case AVSEEK_SIZE:
				{
					return _stream->Length();
				}
			default:
				{
					return -1;
				}
			}
		}
		#pragma endregion
	};
}