#pragma once
#include<ffmpeg-wrapper/AVCalculate.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/filter/VideoFilterGraph.h>
#include<ffmpeg-wrapper/pipe/interface/PipeFrameSource.h>
#include<jccpp/Wrapper.h>
#include<jccpp/container/List.h>
#include<jccpp/container/Queue.h>
#include<jccpp/container/Trigger.h>

namespace video
{
	/**
	 * @brief 用来调整帧率的管道。帧从这里输入后再输出，帧率就变了。这里会插帧或删帧。
	 * 需要注意：改变帧率并不会提升视频质量，反而会降低。
	*/
	class FpsAdjustPipe :
		public IFrameConsumer,
		public IPipeFrameSource
	{
	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="input_video_stream_infos">输入的视频流信息</param>
		/// <param name="desired_out_fps">期望的输出帧率</param>
		FpsAdjustPipe(IVideoStreamInfoCollection &input_video_stream_infos, AVRational desired_out_fps);

	private:
		List<shared_ptr<IFrameConsumer>> _consumer_list;
		VideoStreamInfoCollection _input_video_stream_infos;
		VideoFilterGraph _graph;
		AVRational _desired_out_fps;
		jc::Trigger<int64_t> _trigger;

		void read_and_send_frame();

	public:
		List<shared_ptr<IFrameConsumer>> &ConsumerList() override
		{
			return _consumer_list;
		}

		/// <summary>
		///		* 向 fps 调整管道送入帧。处理完后帧会穿过管道，送给消费者。
		///		离开管道的帧的时间基为 1/fps。此外，输出帧的 pts 会与输入帧进行同步，防止
		///		输入端的 pts 跳跃后输出端仍然保持 pts 的连续，这将会导致音视频不同步。
		/// 
		///		* 如果输出端没有任何消费者，则本函数会直接返回，不执行任何工作。
		/// </summary>
		/// <param name="frame"></param>
		void SendFrame(AVFrameWrapper *frame) override;
	};
}