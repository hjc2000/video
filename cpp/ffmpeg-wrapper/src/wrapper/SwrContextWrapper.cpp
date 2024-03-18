#include "SwrContextWrapper.h"
#include<AVCalculate.h>

using namespace video;

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
