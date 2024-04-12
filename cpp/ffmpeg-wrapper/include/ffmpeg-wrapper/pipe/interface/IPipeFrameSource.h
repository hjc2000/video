#pragma once
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<jccpp/container/List.h>

namespace video
{
	class IPipeFrameSource
	{
	public:
		virtual ~IPipeFrameSource() = default;

	public:
		virtual List<shared_ptr<IFrameConsumer>> &FrameConsumerList() = 0;

		/// <summary>
		///		向每个消费者送入帧。
		/// </summary>
		void SendFrameToEachConsumer(AVFrameWrapper *frame);
	};
}