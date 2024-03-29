#include "SwrContextWrapper.h"
#include<AVCalculate.h>

using namespace video;

SwrContextWrapper::SwrContextWrapper(
	IAudioStreamInfoCollection &in_stream_infos,
	IAudioFrameInfoCollection &out_frame_infos
)
{
	_in_stream_infos = in_stream_infos;
	_out_frame_infos = out_frame_infos;

	int ret = swr_alloc_set_opts2(
		&_wrapped_obj,
		&_out_frame_infos._ch_layout,
		_out_frame_infos._sample_format,
		_out_frame_infos._sample_rate,
		&_in_stream_infos._ch_layout,
		_in_stream_infos._sample_format,
		_in_stream_infos._sample_rate,
		0,
		nullptr
	);

	if (ret < 0)
	{
		cout << CODE_POS_STR << "构造重采样器并设置参数失败" << endl;
		throw jc::Exception();
	}

	ret = swr_init(_wrapped_obj);
	if (ret < 0)
	{
		cout << CODE_POS_STR << "初始化重采样器失败" << endl;
		throw jc::Exception();
	}
}

SwrContextWrapper::~SwrContextWrapper()
{
	cout << "SwrContextWrapper 析构函数" << endl;
	swr_free(&_wrapped_obj);
}

void video::SwrContextWrapper::SendFrame(AVFrameWrapper *input_frame)
{
	lock_guard l(_not_private_methods_lock);

	if (!input_frame)
	{
		// 冲洗模式
		_flushed = true;

		/* _in_pts_when_send_frame 是输出端未来的时间戳，只要将重采样器缓冲区播放完了，
		* 输出端的时间戳就会等于 _in_pts_when_send_frame。当然，这是在输入端的时间基上讨论的，
		* 转换为输出端的时间基，则时间戳不等于 _in_pts_when_send_frame。
		*
		* _in_pts_when_send_frame 每次在 send_frame 中都要被设置为未来的值，但是冲洗模式时不用设置，
		* 因为 _in_pts_when_send_frame 已经是重采样器缓冲区播放完时的值了。
		*/
		int ret = convert(nullptr, 0, nullptr, 0);
		if (ret < 0)
		{
			throw SendFrameException();
		}

		return;
	}

	// 非冲洗模式
	if (input_frame->time_base() != _in_stream_infos.time_base())
	{
		if (!_have_printed_send_frame_rescale_pts_warning)
		{
			cout << CODE_POS_STR << "输入帧的时间基和构造本对象时设置的输入流时间基不同，进行 pts 缩放。" << endl;
			_have_printed_send_frame_rescale_pts_warning = true;
		}

		int64_t rescaled_input_frame_pts = ConvertTimeStamp(
			input_frame->pts(),
			input_frame->time_base(),
			_in_stream_infos.time_base()
		);

		input_frame->set_pts(rescaled_input_frame_pts);
	}

	_in_pts_when_send_frame = input_frame->pts();
	int ret = convert(nullptr, 0, (*input_frame)->extended_data, input_frame->nb_samples());
	if (ret < 0)
	{
		throw SendFrameException();
	}
}

int video::SwrContextWrapper::ReadFrame(AVFrameWrapper &output_frame)
{
	lock_guard l(_not_private_methods_lock);
	output_frame = AVFrameWrapper{ _out_frame_infos };

	int ret = 0;
	if (_flushed)
	{
		// 冲洗模式
		ret = read_frame_in_flushing_mode(output_frame);
	}
	else
	{
		// 非冲洗模式
		ret = read_frame_in_non_flushing_mode(output_frame);
	}

	/* 在送入帧的时候记录了当时的 pts，为 _pts_when_send_frame。现在已经读取出 output_frame 了，
	* 将 _in_pts_when_send_frame 减去重采样器内，以输入侧时间基为时间基的延迟，就可以得到 output_frame
	* 的 pts 在输入侧的值。因为延迟的关系，它比 _in_pts_when_send_frame 小。
	*
	* 但是，in_pts 是输入侧的时间戳，我们需要转换为在输出侧的时间戳，然后赋值给 output_frame。
	*/
	int64_t delay = get_delay(_in_stream_infos.time_base().den / _in_stream_infos.time_base().num);
	if (ret == (int)ErrorCode::eof)
	{
		/* 不清楚冲洗完后重采样器内会不会仍然延迟不为 0，所以冲洗后，并且返回 eof，此时表示重采样器空了。
		* 手动将 delay 设为 0.
		*/
		delay = 0;
	}

	int64_t in_pts = _in_pts_when_send_frame - delay;
	int64_t out_pts = ConvertTimeStamp(
		in_pts,
		_in_stream_infos.time_base(),
		_out_frame_infos.time_base()
	);
	output_frame.set_time_base(_out_frame_infos.time_base());
	output_frame.set_pts(out_pts);
	output_frame.set_duration(0);
	return ret;
}

int SwrContextWrapper::convert(uint8_t **out, int out_count, uint8_t **in, int in_count)
{
	int ret = swr_convert(
		_wrapped_obj,
		out,
		out_count,
		(const uint8_t **)in,
		in_count
	);

	return ret;
}

int SwrContextWrapper::convert(AVFrameWrapper *input_frame, AVFrameWrapper *output_frame)
{
	uint8_t **in = input_frame ? (*input_frame)->extended_data : nullptr;
	int in_count = input_frame ? input_frame->nb_samples() : 0;

	uint8_t **out = output_frame ? (*output_frame)->extended_data : nullptr;
	int out_count = output_frame ? output_frame->nb_samples() : 0;

	// 重采样
	int ret = convert(out, out_count, in, in_count);

	return ret;
}

int SwrContextWrapper::read_frame_in_flushing_mode(AVFrameWrapper &output_frame)
{
	int count = convert(nullptr, &output_frame);
	cout << "冲洗缓冲区冲洗出了" << count << "个采样点" << endl;

	// 如果有填充（即 count > 0）且填充了不完整的帧
	if (count > 0 && count < output_frame.nb_samples())
	{
		// 将后面没被填充的采样点设置为静音
		output_frame.mute(count);
		cout << "填充了不完整的帧，已将后面未填充的采样点设置为静音" << endl;
	}

	if (count > 0)
	{
		/* 有填充，说明重采样器内还是有采样点的，返回 0，表示读取成功。
		* 只不过这个读取成功的帧有可能尾部是被填充了静音采样点的。
		*/
		return 0;
	}

	// 一个采样点都没填充，本次读取帧失败，返回 eof。
	return (int)ErrorCode::eof;
}

int SwrContextWrapper::read_frame_in_non_flushing_mode(AVFrameWrapper &output_frame)
{
	if (can_fill_output_frame(output_frame))
	{
		// 可以填充一个完整的帧
		int count = convert(nullptr, &output_frame);
		if (count != output_frame.nb_samples())
		{
			throw jc::Exception("read_frame 没有填充完整的 output_frame，本来认为这里一定会填充完整的帧的。");
		}

		return 0;
	}

	// 无法填充一个完整的帧，所以一个采样点都不填充
	// 返回 -11 表示输出暂时不可用，需要更多的输入数据。这是官方的错误代码。
	return (int)ErrorCode::output_is_temporarily_unavailable;
}

int SwrContextWrapper::send_silence_samples(uint32_t nb_samples)
{
	lock_guard l(_not_private_methods_lock);
	if (nb_samples == 0)
	{
		return 0;
	}

	if (!_silence_frame)
	{
		_silence_frame = shared_ptr<AVFrameWrapper>{ new AVFrameWrapper{_in_stream_infos,64} };
		_silence_frame->mute(0);
	}

	// 循环次数
	uint32_t loop_times = nb_samples / _silence_frame->nb_samples();
	for (uint32_t i = 0; i < loop_times; i++)
	{
		SendFrame(_silence_frame.get());
	}

	// 求模，取余数，看用 _silence_frame 填充 loop_times 次后还会剩下几个采样点才能达到 nb_samples
	uint32_t remain_nb_samples = nb_samples % _silence_frame->nb_samples();
	int ret = convert(nullptr,
		0,
		(*_silence_frame)->extended_data,
		remain_nb_samples);
	if (ret < 0)
	{
		// 失败返回负数的错误代码
		return ret;
	}

	// 成功返回 0.
	return 0;
}
