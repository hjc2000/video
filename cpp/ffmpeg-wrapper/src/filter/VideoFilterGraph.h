#pragma once
#include<AVDictionaryWrapper.h>
#include<AVFilterContextWrapper.h>
#include<IFrameConsumer.h>
#include<IFrameSource.h>
#include<ffmpeg-wrapper/info-collection/VideoStreamInfoCollection.h>

namespace video
{
	class VideoFilterGraph :
		public Wrapper<AVFilterGraph>,
		public IFrameConsumer,
		public IFrameSource
	{
	public:
		VideoFilterGraph(IVideoStreamInfoCollection &infos);

	private:
		VideoStreamInfoCollection _input_video_stream_infos;

		AVFilterContextWrapper _buffer_filter;
		AVFilterContextWrapper _buffer_sink_filter;
		void init_buffer_filter();
		void init_buffer_sink_filter();

	public:
		/// <summary>
		///		获取本滤镜图的起点 —— buffer_filter
		/// </summary>
		/// <returns></returns>
		AVFilterContextWrapper &buffer_filter();

		/// <summary>
		///		获取本滤镜图的终点 —— buffer_sink_filter
		/// </summary>
		/// <returns></returns>
		AVFilterContextWrapper &buffer_sink_filter();

		/// <summary>
		///		构造一个 fps 滤镜
		/// </summary>
		/// <param name="frame_rate"></param>
		/// <returns></returns>
		AVFilterContextWrapper alloc_fps_filter(AVRational frame_rate);

		void config_graph();

		int ReadFrame(AVFrameWrapper &frame) override;
		void SendFrame(AVFrameWrapper *frame) override;
	};
}
