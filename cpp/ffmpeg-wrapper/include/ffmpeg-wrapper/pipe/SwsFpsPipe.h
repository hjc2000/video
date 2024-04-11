#pragma once
#include<ffmpeg-wrapper/pipe/FpsAdjustPipe.h>
#include<ffmpeg-wrapper/pipe/SwsPipe.h>
#include<ffmpeg-wrapper/pipe/interface/PipeFrameSource.h>

namespace video
{
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
		void AddFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer) override;
		bool RemoveFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer) override;
		void ClearFrameConsumer() override;
	};
}