#include "AudioFramePlayer.h"
#include<AVCodecContextWrapper.h>
#include<AVStreamWrapper.h>
#include<InputFormatContext.h>

using namespace video;
using namespace video;

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
