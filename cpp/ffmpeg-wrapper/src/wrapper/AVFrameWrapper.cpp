#include "AVFrameWrapper.h"
#include<ImageBuffer.h>
#include<include_ffmpeg.h>

using namespace video;

int video::AVFrameWrapper::audio_data_size()
{
	/*  平面类型的声道布局，每一个声道的缓冲区后面都会有间隙。包类型的声道布局，多个声道的采样值
	* 交错存放，呈 LRLRLR......
	*   包类型的声道布局的缓冲区中，只有在最末尾有因为对齐产生的间隙；平面类型的声道布局的缓冲区中，
	* 每一个声道的缓冲区后面都有因为对齐产生的间隙。
	*   PCM 文件需要包类型的声道布局。平面类型的声道布局仅限于 ffmpeg 内部各个函数进行数据交换
	* 使用，不能写到文件中。
	*   此外，要将缓冲区写入 PCM 文件时，后面的间隙不能写入文件。PCM 文件中的每一个数据都是实际的
	* 采样值，不能是间隙，因为读的时候也是认为没有间隙的。如果把间隙写入 PCM 文件，播放的时候就会有噪音，
	* 声音的时间也会不对。
	*   综上，下面应该向 av_samples_get_buffer_size 函数的 align 参数传入 1，表示不对齐，也就是
	* 获取的 buf_size 不包括后面的间隙。
	*/
	int buf_size = av_samples_get_buffer_size(
		nullptr,
		ch_layout().nb_channels,
		nb_samples(),
		sample_format(),
		1
	);

	return buf_size;
}

void video::AVFrameWrapper::mute(int offset)
{
	av_samples_set_silence(
		_wrapped_obj->extended_data,
		offset,
		nb_samples() - offset,
		ch_layout().nb_channels,
		sample_format()
	);
}

void AVFrameWrapper::get_buffer(int align)
{
	int ret = ::av_frame_get_buffer(_wrapped_obj, align);
	if (ret < 0)
	{
		cout << CODE_POS_STR << "av_frame_get_buffer 失败。" << endl;
		throw jc::Exception();
	}
}

void AVFrameWrapper::make_writable()
{
	int ret = ::av_frame_make_writable(_wrapped_obj);
	if (ret)
	{
		cout << CODE_POS_STR << "av_frame_make_writable 失败。" << endl;
		throw jc::Exception();
	}
}

void AVFrameWrapper::copy_image_to_buffer(shared_ptr<ImageBuffer> buffer)
{
	av_image_copy(
		buffer->_pointers,
		buffer->_linesizes,
		(const uint8_t **)(_wrapped_obj->data),
		_wrapped_obj->linesize,
		(AVPixelFormat)_wrapped_obj->format,
		_wrapped_obj->width,
		_wrapped_obj->height
	);
}

AVSampleFormat video::AVFrameWrapper::sample_format()
{
	return (AVSampleFormat)_wrapped_obj->format;
}

void video::AVFrameWrapper::set_sample_format(AVSampleFormat value)
{
	_wrapped_obj->format = value;
}

int video::AVFrameWrapper::nb_samples()
{
	return _wrapped_obj->nb_samples;
}

void video::AVFrameWrapper::set_nb_samples(int value)
{
	_wrapped_obj->nb_samples = value;
}

AVChannelLayout video::AVFrameWrapper::ch_layout()
{
	return _wrapped_obj->ch_layout;
}

void video::AVFrameWrapper::set_ch_layout(AVChannelLayout value)
{
	_wrapped_obj->ch_layout = value;
}

int video::AVFrameWrapper::sample_rate()
{
	return _wrapped_obj->sample_rate;
}

void video::AVFrameWrapper::set_sample_rate(int value)
{
	_wrapped_obj->sample_rate = value;
}

void video::AVFrameWrapper::CopyVideoFrameToStream(Stream &stream)
{
	if (!_image_buf)
	{
		/* yuv 文件不能有间隙。对齐会产生间隙，一旦有间隙，间隙也会被理解成像素，就会导致花屏，
		* 根本无法正常播放。所以这里必须使用 1 字节对齐（即不对齐）。不管写入什么流都是一样的，
		* 统一写入不对齐的数据，包括网络流。
		*/
		_image_buf = shared_ptr<ImageBuffer>(
			new ImageBuffer(
				_wrapped_obj->width, _wrapped_obj->height,
				(AVPixelFormat)_wrapped_obj->format, 1
			)
		);
	}

	copy_image_to_buffer(_image_buf);
	_image_buf->CopyToStream(stream);
}

void video::AVFrameWrapper::CopyAudioFrameToStream(Stream &stream)
{
	int buf_size = audio_data_size();
	if (buf_size < 0)
	{
		throw jc::Exception();
	}

	stream.Write(_wrapped_obj->extended_data[0], 0, buf_size);
}

int video::AVFrameWrapper::width()
{
	return _wrapped_obj->width;
}

void video::AVFrameWrapper::set_width(int value)
{
	_wrapped_obj->width = value;
}

int video::AVFrameWrapper::height()
{
	return _wrapped_obj->height;
}

void video::AVFrameWrapper::set_height(int value)
{
	_wrapped_obj->height = value;
}

AVPixelFormat video::AVFrameWrapper::pixel_format()
{
	return AVPixelFormat(_wrapped_obj->format);
}

void video::AVFrameWrapper::set_pixel_format(AVPixelFormat value)
{
	_wrapped_obj->format = value;
}

AVRational video::AVFrameWrapper::time_base()
{
	return _wrapped_obj->time_base;
}

void video::AVFrameWrapper::set_time_base(AVRational value)
{
	_wrapped_obj->time_base = value;
}
