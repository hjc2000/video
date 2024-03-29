#include "DecoderPipe.h"

using namespace video;

DecoderPipe::DecoderPipe(AVStreamInfoCollection stream)
{
	_stream_infos = stream;
	_decoder = AVCodecContextWrapper::CreateDecoder(stream);
}

DecoderPipe::~DecoderPipe()
{
	Dispose();
}

void DecoderPipe::Dispose()
{
	if (_disposed) return;
	_disposed = true;
}

shared_ptr<AVCodecContextWrapper> DecoderPipe::DecoderContext()
{
	return _decoder;
}

void DecoderPipe::SendPacket(AVPacketWrapper *packet)
{
	if (_consumer_list.Count() == 0)
	{
		// 管道出口没有接收者。直接返回，节省性能。
		return;
	}

	// 防止解码器中有数据残留，会导致送入包失败
	read_and_send_frame();

	if (!packet)
	{
		// 冲洗解码器
		_decoder->SendPacket(nullptr);
		read_and_send_frame();
		return;
	}

	// packet 不是空指针
	if (packet->StreamIndex() != _stream_infos._index)
	{
		// 索引不匹配，直接返回。
		return;
	}

	_decoder->SendPacket(packet);
	read_and_send_frame();
}

void DecoderPipe::read_and_send_frame()
{
	while (!_disposed)
	{
		int ret = _decoder->ReadFrame(_decoder_out_frame);
		switch (ret)
		{
		case 0:
			{
				SendFrameToEachConsumer(&_decoder_out_frame);
				break;
			}
		case (int)ErrorCode::output_is_temporarily_unavailable:
			{
				// 解码器需要继续输入包才能输出帧
				return;
			}
		case (int)ErrorCode::eof:
			{
				// 解码器已被冲洗
				SendFrameToEachConsumer(nullptr);
				return;
			}
		default:
			{
				// 未知错误
				throw jc::Exception();
			}
		}
	}
}

void DecoderPipe::FlushDecoderButNotFlushConsumers()
{
	_decoder->SendPacket(nullptr);
	while (!_disposed)
	{
		int ret = _decoder->ReadFrame(_decoder_out_frame);
		switch (ret)
		{
		case 0:
			{
				SendFrameToEachConsumer(&_decoder_out_frame);
				break;
			}
		case (int)ErrorCode::output_is_temporarily_unavailable:
			{
				// 解码器需要继续输入包才能输出帧
				return;
			}
		case (int)ErrorCode::eof:
			{
				// 解码器已被冲洗，但这里不冲洗消费者。
				return;
			}
		default:
			{
				// 未知错误
				throw jc::Exception();
			}
		}
	}
}

#pragma region IAudioStreamInfoCollection, IVideoStreamInfoCollection
AVRational video::DecoderPipe::time_base()
{
	return _stream_infos._timebase;
}

void video::DecoderPipe::set_time_base(AVRational value)
{
	_stream_infos._timebase = value;
}

AVSampleFormat video::DecoderPipe::sample_format()
{
	return _stream_infos._sample_format;
}

void video::DecoderPipe::set_sample_format(AVSampleFormat value)
{
	_stream_infos._sample_format = value;
}

int video::DecoderPipe::sample_rate()
{
	return _stream_infos._sample_rate;
}

void video::DecoderPipe::set_sample_rate(int value)
{
	_stream_infos._sample_rate = value;
}

AVChannelLayout video::DecoderPipe::ch_layout()
{
	return _stream_infos._ch_layout;
}

void video::DecoderPipe::set_ch_layout(AVChannelLayout value)
{
	_stream_infos._ch_layout = value;
}

int video::DecoderPipe::width()
{
	return _stream_infos._width;
}

void video::DecoderPipe::set_width(int value)
{
	_stream_infos._width = value;
}

int video::DecoderPipe::height()
{
	return _stream_infos._height;
}

void video::DecoderPipe::set_height(int value)
{
	_stream_infos._height = value;
}

AVPixelFormat video::DecoderPipe::pixel_format()
{
	return _stream_infos._pixel_format;
}

void video::DecoderPipe::set_pixel_format(AVPixelFormat value)
{
	_stream_infos._pixel_format = value;
}

AVRational video::DecoderPipe::frame_rate()
{
	return _stream_infos._frame_rate;
}

void video::DecoderPipe::set_frame_rate(AVRational value)
{
	_stream_infos._frame_rate = value;
}
#pragma endregion

