#pragma once
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/PipeFrameSource.h>

namespace video
{
	class SwsPipe;
	class FpsAdjustPipe;

	class SwsFpsPipe :
		public IFrameConsumer,
		public IPipeFrameSource
	{
	public:
		SwsFpsPipe(IVideoStreamInfoCollection &out_video_stream_infos);

	private:
		shared_ptr<SwsPipe> _sws_pipe;
		shared_ptr<FpsAdjustPipe> _fps_adjust_pipe;

	public:
		List<shared_ptr<IFrameConsumer>> &ConsumerList() override;
		void SendFrame(AVFrameWrapper *frame) override;
	};
}