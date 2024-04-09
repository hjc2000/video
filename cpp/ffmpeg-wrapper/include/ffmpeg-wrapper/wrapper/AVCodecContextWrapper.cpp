#include"ffmpeg-wrapper/wrapper/AVCodecContextWrapper.h"
#include<ffmpeg-wrapper/AVCalculate.h>
#include<ffmpeg-wrapper/AVCodecExtention.h>
#include<ffmpeg-wrapper/wrapper/AVDictionaryWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<jccpp/Exception.h>
#include<format>
#include<ffmpeg-wrapper/base_include.h>

using namespace video;

#pragma region 构造，析构
AVCodecContextWrapper::AVCodecContextWrapper(AVCodec const *codec)
{
	SetCodec(codec);
	_wrapped_obj = ::avcodec_alloc_context3(codec);
	if (!_wrapped_obj)
	{
		throw jc::Exception("AVCodecContextWrapper create(AVCodecWrapper codec) 失败");
	}
}

AVCodecContextWrapper::AVCodecContextWrapper(const AVCodec *codec, AVCodecParameters *param)
	: AVCodecContextWrapper(codec)
{
	SetCodecParam(param);
}

AVCodecContextWrapper::~AVCodecContextWrapper()
{
	cout << __func__ << endl;
	avcodec_free_context(&_wrapped_obj);
}
#pragma endregion

#pragma region 工厂函数
shared_ptr<AVCodecContextWrapper> AVCodecContextWrapper::CreateDecoder(AVStreamInfoCollection stream)
{
	shared_ptr<AVCodecContextWrapper> ctx{
		new AVCodecContextWrapper{
			stream._codec,
			stream._codec_params
	}
	};

	ctx->set_time_base(stream.time_base());
	ctx->set_frame_rate(stream.frame_rate());

	ctx->Open();
	return ctx;
}

shared_ptr<AVCodecContextWrapper> AVCodecContextWrapper::CreateEncoder(
	const char *encoder_name,
	IAudioStreamInfoCollection &infos,
	bool set_global_header,
	bool auto_open
)
{
	auto codec = AVCodecExtention::find_encoder_by_name(encoder_name);
	if (!codec)
	{
		cout << CODE_POS_STR << "查找编码器失败" << endl;
		throw jc::Exception();
	}

	shared_ptr<AVCodecContextWrapper> ctx{
		new AVCodecContextWrapper{
			codec,
	}
	};

	// 设置编码器参数
	(*ctx)->codec_type = AVMediaType::AVMEDIA_TYPE_AUDIO;
	(*ctx)->ch_layout = infos.ch_layout();
	(*ctx)->sample_fmt = infos.sample_format();
	(*ctx)->sample_rate = infos.sample_rate();
	(*ctx)->time_base = infos.time_base();
	if (set_global_header)
	{
		ctx->SetGlobalHeader();
	}

	if (auto_open)
	{
		ctx->Open();
	}

	return ctx;
}

shared_ptr<AVCodecContextWrapper> AVCodecContextWrapper::CreateEncoder(
	const char *encoder_name,
	IVideoStreamInfoCollection &infos,
	bool set_global_header,
	bool auto_open
)
{
	auto codec = AVCodecExtention::find_encoder_by_name(encoder_name);
	if (!codec)
	{
		cout << CODE_POS_STR << "查找编码器失败" << endl;
		throw jc::Exception();
	}

	shared_ptr<AVCodecContextWrapper> ctx{
		new AVCodecContextWrapper{
			codec,
	}
	};

	// 设置编码器参数
	(*ctx)->codec_type = AVMediaType::AVMEDIA_TYPE_VIDEO;

	(*ctx)->width = infos.width();
	(*ctx)->height = infos.height();
	(*ctx)->pix_fmt = infos.pixel_format();

	(*ctx)->time_base = infos.time_base();
	(*ctx)->framerate = infos.frame_rate();

	//(*ctx)->max_b_frames = 10;
	//(*ctx)->gop_size = 60;
	if (set_global_header)
	{
		ctx->SetGlobalHeader();
	}

	if (auto_open)
	{
		ctx->Open();
	}

	return ctx;
}
#pragma endregion

void AVCodecContextWrapper::SetCodecParam(AVCodecParameters *param)
{
	int ret = ::avcodec_parameters_to_context(_wrapped_obj, param);
	if (ret < 0)
	{
		throw jc::Exception("AVCodecContextWrapper::set_encoder_param");
	}
}

void AVCodecContextWrapper::Open(AVDictionary **dic)
{
	int ret = ::avcodec_open2(_wrapped_obj, _codec, dic);
	if (ret)
	{
		std::cerr << CODE_POS_STR << "打开编解码器失败" << endl;
		throw jc::Exception();
	}
}

#pragma region 编码
void AVCodecContextWrapper::SendFrame(AVFrameWrapper *frame)
{
	int ret;
	if (frame)
	{
		// 正常送入帧
		if (frame->time_base() != time_base())
		{
			frame->ChangeTimeBase(time_base());
		}

		frame->make_writable();
		ret = ::avcodec_send_frame(_wrapped_obj, *frame);
	}
	else
	{
		// 送入空指针，表示冲洗编码器
		ret = ::avcodec_send_frame(_wrapped_obj, nullptr);
	}

	if (ret)
	{
		std::string msg = std::format(
			"送入帧失败，错误代码：{} —— {}",
			ret,
			ToString((ErrorCode)ret)
		);

		cout << CODE_POS_STR << msg << endl;
		throw video::SendFrameException(msg);
	}
}

int AVCodecContextWrapper::ReadPacket(AVPacketWrapper &packet)
{
	int ret = ::avcodec_receive_packet(_wrapped_obj, packet);
	if (!ret)
	{
		packet.set_time_base(time_base());
	}

	return ret;
}
#pragma endregion

#pragma region 解码
void AVCodecContextWrapper::SendPacket(AVPacketWrapper *packet)
{
	int ret;
	if (packet)
	{
		/* 解码不需要对包进行时间基调整。AVCodecContext 在作为解码器的时候自己会记录时间基，将所有
		* 包的时间基都当作这个时间基。从封装中读出来的包不含有时间基信息（时间基字段为：0 / 1 ，是个无效值），
		* 只含有 pst 和 dts。
		*/
		ret = ::avcodec_send_packet(_wrapped_obj, *packet);
	}
	else
	{
		ret = ::avcodec_send_packet(_wrapped_obj, nullptr);
	}

	if (ret < 0)
	{
		cout << CODE_POS_STR << "错误代码：" << ret << " -- " << ToString((ErrorCode)ret);
		//throw SendPacketException{};
	}
}

int AVCodecContextWrapper::ReadFrame(AVFrameWrapper &frame)
{
	// avcodec_receive_frame 内部在执行工作之前会先调用 av_frame_unref
	int ret = avcodec_receive_frame(_wrapped_obj, frame);
	if (!ret)
	{
		// 解码出来的帧会被设置时间戳，但是时间基不会被设置。这里补充设置。
		frame.set_time_base(time_base());
	}

	return ret;
}
#pragma endregion

#pragma region IAudioStreamInfoCollection, IVideoStreamInfoCollection
AVChannelLayout AVCodecContextWrapper::ch_layout()
{
	return _wrapped_obj->ch_layout;
}

void AVCodecContextWrapper::set_ch_layout(AVChannelLayout value)
{
	_wrapped_obj->ch_layout = value;
}

AVSampleFormat AVCodecContextWrapper::sample_format()
{
	return _wrapped_obj->sample_fmt;
}

void AVCodecContextWrapper::set_sample_format(AVSampleFormat value)
{
	_wrapped_obj->sample_fmt = value;
}

int AVCodecContextWrapper::sample_rate()
{
	return _wrapped_obj->sample_rate;
}

void AVCodecContextWrapper::set_sample_rate(int value)
{
	_wrapped_obj->sample_rate = value;
}

int AVCodecContextWrapper::width()
{
	return _wrapped_obj->width;
}
void AVCodecContextWrapper::set_width(int value)
{
	_wrapped_obj->width = value;
}

int AVCodecContextWrapper::height()
{
	return _wrapped_obj->height;
}
void AVCodecContextWrapper::set_height(int value)
{
	_wrapped_obj->height = value;
}

AVPixelFormat AVCodecContextWrapper::pixel_format()
{
	return _wrapped_obj->pix_fmt;
}

void AVCodecContextWrapper::set_pixel_format(AVPixelFormat value)
{
	_wrapped_obj->pix_fmt = value;
}

AVRational AVCodecContextWrapper::time_base()
{
	return _wrapped_obj->time_base;
}

void AVCodecContextWrapper::set_time_base(AVRational value)
{
	_wrapped_obj->time_base = value;
}

AVRational AVCodecContextWrapper::frame_rate()
{
	return _wrapped_obj->framerate;
}

void AVCodecContextWrapper::set_frame_rate(AVRational value)
{
	_wrapped_obj->framerate = value;
}
#pragma endregion
