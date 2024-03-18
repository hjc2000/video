#pragma once
#include<IFrameConsumer.h>
#include<PipeFrameSource.h>

namespace video
{
	/**
	 * @brief 透传的帧管道。送进来的帧不会经过任何加工，直接送给消费者。
	*/
	class PassThoughFramePipe :public PipeFrameSource, public IFrameConsumer
	{
	public:
		void SendFrame(AVFrameWrapper *frame) override
		{
			SendFrameToEachConsumer(frame);
		}
	};
}