#pragma once
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IPipeFrameSource.h>

namespace video
{
	class SwsPipe;
	class FpsAdjustPipe;

	class SwsFpsPipe :
		public IFrameConsumer,
		public IPipeFrameSource
	{
		shared_ptr<SwsPipe> _sws_pipe;
		shared_ptr<FpsAdjustPipe> _fps_adjust_pipe;

	public:
		SwsFpsPipe(IVideoStreamInfoCollection &out_video_stream_infos);
		List<shared_ptr<IFrameConsumer>> &FrameConsumerList() override;
		void SendFrame(AVFrameWrapper *frame) override;
	};
}