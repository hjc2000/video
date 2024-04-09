#include "ffmpeg-wrapper/wrapper/SwsContextWrapper.h"
#include<ffmpeg-wrapper/wrapper/SwrContextWrapper.h>

using namespace video;

video::SwsContextWrapper::SwsContextWrapper(
	IVideoFrameInfoCollection &in_video_frame_infos,
	IVideoFrameInfoCollection &out_video_frame_infos
)
{
	_in_video_frame_infos = in_video_frame_infos;
	_out_video_frame_infos = out_video_frame_infos;

	_wrapped_obj = sws_getContext(
		in_video_frame_infos.width(),
		in_video_frame_infos.height(),
		in_video_frame_infos.pixel_format(),
		out_video_frame_infos.width(),
		out_video_frame_infos.height(),
		out_video_frame_infos.pixel_format(),
		SWS_FAST_BILINEAR,// 使用双线性快速滤波算法
		nullptr,
		nullptr,
		nullptr
	);

	if (!_wrapped_obj)
	{
		cout << CODE_POS_STR << "构造 sws 失败" << endl;
		throw jc::Exception();
	}
}

void video::SwsContextWrapper::SendFrame(AVFrameWrapper *frame)
{
	std::lock_guard l(_lock);
	if (_flushed)
	{
		cout << CODE_POS_STR << "冲洗后不允许再送入" << endl;
		throw jc::Exception();
	}

	if (!frame)
	{
		_flushed = true;
		cout << CODE_POS_STR << "sws 已被冲洗" << endl;
		return;
	}

	if (_out_frame_avaliable)
	{
		cout << CODE_POS_STR << "还没有读出帧却继续送入帧" << endl;
		throw jc::Exception();
	}

	_in_frame = *frame;
	_out_frame_avaliable = true;
}

int video::SwsContextWrapper::ReadFrame(AVFrameWrapper &frame)
{
	std::lock_guard l(_lock);
	if (_flushed && !_out_frame_avaliable)
	{
		return (int)ErrorCode::eof;
	}

	if (!_out_frame_avaliable)
	{
		return (int)ErrorCode::output_is_temporarily_unavailable;
	}

	_out_frame_avaliable = false;

	frame = AVFrameWrapper{ _out_video_frame_infos };
	frame.set_time_base(_in_frame.time_base());
	frame.set_pts(_in_frame.pts());

	int ret = sws_scale(_wrapped_obj,
		_in_frame->data,
		_in_frame->linesize,
		0,
		_in_frame->height,
		frame->data,
		frame->linesize
	);

	_in_frame.unref();
	return 0;
}
