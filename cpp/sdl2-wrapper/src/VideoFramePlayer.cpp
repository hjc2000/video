#include"sdl2-wrapper/VideoFramePlayer.h"

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

VideoFramePlayer::~VideoFramePlayer()
{
	Dispose();
	cout << "~VideoFramePlayer()" << endl;
}

void VideoFramePlayer::Dispose()
{
	if (_disposed) return;
	_disposed = true;

	_frame_queue.Dispose();
	_timer.Stop();
}

shared_ptr<IRefTimer> VideoFramePlayer::RefTimer()
{
	return _ref_timer;
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

	frame.set_time_base(_video_stream_infos.TimeBase());
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

	int64_t next_interval = _video_stream_infos.FrameIntervalInMilliseconds() + e_t;
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

void video::VideoFramePlayer::Pause(bool pause)
{
	/* 这里使用的是原子量，且 _timer 的方法是线程安全的，_video_frame_infos 又不会在另一个线程中
	* 被修改 frame_interval_in_milliseconds 属性，所以不用竞争 _not_private_methods_lock。
	*/
	if (pause)
	{
		// 暂停播放
		_timer.StopNoWait();
		return;
	}

	// 开始播放
	uint32_t interval = _video_stream_infos.FrameIntervalInMilliseconds();
	cout << "开始播放，帧间隔为：" << interval << endl;
	_timer.Start(interval);
}

void video::VideoFramePlayer::SendFrame(AVFrameWrapper *frame)
{
	if (!frame)
	{
		_frame_queue.Flush();
		return;
	}

	_frame_queue.Enqueue(*frame);
	return;
}

void VideoFramePlayer::SetRefTimer(shared_ptr<IRefTimer> value)
{
	lock_guard l(_ref_timer_lock);
	_ref_timer = value;
}

int64_t VideoFramePlayer::RefTime()
{
	lock_guard l(_ref_timer_lock);
	return _ref_timer->RefTime();
}
