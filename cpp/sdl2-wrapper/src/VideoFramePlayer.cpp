#include"VideoFramePlayer.h"

using namespace video;
using namespace std;

video::VideoFramePlayer::VideoFramePlayer(
	int x,
	int y,
	IVideoStreamInfoCollection &infos,
	std::string window_title,
	SDL_WindowFlags flags
)
{
	_video_stream_infos = infos;
	_displayer = shared_ptr<VideoFrameDisplayer>{
		new VideoFrameDisplayer{
			x,
			y,
			_video_stream_infos.width(),
			_video_stream_infos.height(),
			_video_stream_infos.pixel_format(),
			window_title,
			flags,
		}
	};

	_timer._callback = [&](uint32_t interval_in_milliseconds)->uint32_t
	{
		return SDL_TimerCallbackHandler(interval_in_milliseconds);
	};
}

uint32_t video::VideoFramePlayer::SDL_TimerCallbackHandler(uint32_t interval_in_milliseconds)
{
	AVFrameWrapper frame;
	try
	{
		frame = _frame_queue.Dequeue();
	}
	catch (jc::InvalidOperationException &)
	{
		cout << CODE_POS_STR << "_frame_queue.Dequeue() 抛出异常。停止 SDL 定时器" << endl;
		return 0;
	}

	_displayer->SendFrame(&frame);

	frame.set_time_base(_video_stream_infos.time_base());
	int64_t video_time = frame.PtsToMilliseconds().count();
	if (!_ref_timer)
	{
		// 不进行音视频同步
		return interval_in_milliseconds;
	}

	// 需要进行音视频同步
	int64_t ref_time = RefTime();
	int64_t e_t = video_time - ref_time;
	cout << e_t << endl;

	int64_t next_interval = _video_stream_infos.frame_interval_in_milliseconds<uint32_t>() + e_t;
	if (next_interval <= 0)
	{
		/*
		* 如果对 SDL 返回负数，因为 SDL 按无符号处理，所以会导致相当长的定时时间。
		* 如果对 SDL 返回 0 会导致停止定时器。
		* 所以返回 1，这是允许的最小值了。
		*/
		next_interval = 1;
	}

	return (uint32_t)next_interval;
}
