#include "AudioFramePlayer.h"
#include<AVCodecContextWrapper.h>
#include<AVStreamWrapper.h>
#include<InputFormat.h>

using namespace video;
using namespace video;

AudioFramePlayer::AudioFramePlayer(IAudioStreamInfoCollection &infos)
{
	_time_base = infos.time_base();
	_device = shared_ptr<SDL_DefaultAudioDevice>{ new SDL_DefaultAudioDevice{} };
	cout << _device->ToString() << endl;

	_device->_audio_callback = [&](uint8_t *stream, int len)
	{
		AudioCallbackHandler(stream, len);
	};

	_swr_pipe = shared_ptr<SwrPipe>{ new SwrPipe {*_device} };
	_swr_pipe->AddFrameConsumer(_frame_queue);
}

AudioFramePlayer::~AudioFramePlayer()
{
	Dispose();
	cout << "~AudioFramePlayer()" << endl;
}

void AudioFramePlayer::Dispose()
{
	if (_disposed) return;
	_disposed = true;

	_frame_queue->Dispose();
	_device->Dispose();
}

void AudioFramePlayer::AudioCallbackHandler(uint8_t *stream, int len)
{
	if (_should_pause || _disposed)
	{
		return;
	}

	// 尝试从重采样器中读取帧
	int ret = _frame_queue->ReadFrame(_swr_out_frame);
	if (ret)
	{
		// 发生了未知错误，或者发生了 eof，即播放器已经被冲洗了。
		/* 冲洗播放器的方法是调用本播放器对象的 send_frame 方法时传入空指针，
		* 这将冲洗重采样器。
		*/
		Pause(true);
		cout << "发生了错误或到达文件尾，AudioFramePlayer 停止播放" << endl;
		return;
	}

	// 读取成功
	_swr_out_frame.CopyAudioDataToBuffer(stream, len);
	_time_interpolator.SyncTime(_swr_out_frame.PtsToMilliseconds().count());
}

int64_t AudioFramePlayer::RefTime()
{
	return _time_interpolator.InterpolatedTimeInMilliseconds();
}

void AudioFramePlayer::Pause(bool pause)
{
	/* 这里没什么要加锁的，设置标志位而已。而且标志位还用了原子量。_device->Pause
	* 也是线程安全的。
	*/
	if (pause)
	{
		// 暂停播放
		_should_pause = true;
		_device->Pause(true);
		_time_interpolator.Pause();
	}
	else
	{
		// 启动播放（第一次启动或暂停后再次启动）
		_should_pause = false;
		_device->Pause(false);
	}
}

void AudioFramePlayer::SendFrame(AVFrameWrapper *frame)
{
	if (_disposed)
	{
		throw jc::ObjectDisposedException();
	}

	try
	{
		_swr_pipe->SendFrame(frame);
	}
	catch (jc::InvalidOperationException &)
	{
		cout << CODE_POS_STR << "帧队列已冲洗，无法送入帧" << endl;
	}
	catch (jc::ObjectDisposedException &)
	{
		cout << CODE_POS_STR << "帧队列已释放，无法送入帧" << endl;
	}
}
