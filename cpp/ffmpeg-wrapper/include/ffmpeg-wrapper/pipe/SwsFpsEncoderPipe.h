#pragma once
#include<ffmpeg-wrapper/info-collection/VideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/output-format/OutputFormat.h>
#include<ffmpeg-wrapper/pipe/EncoderPipe.h>
#include<ffmpeg-wrapper/pipe/FpsAdjustPipe.h>
#include<ffmpeg-wrapper/pipe/SwsPipe.h>

namespace video
{
	class SwsFpsEncoderPipe :public IFrameConsumer
	{
	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="desire_out_video_stream_infos"></param>
		/// <param name="encoder_pipe">
		///		自定义的编码管道。例如对于特定的开发板，将硬件编码器封装，实现 IFrameConsumer 接口，
		///		就可以将该对象传给本构造函数，这样就能利用这个编码器了。
		/// </param>
		SwsFpsEncoderPipe(
			IVideoStreamInfoCollection &desire_out_video_stream_infos,
			shared_ptr<IFrameConsumer> encoder_pipe
		);

		/// <summary>
		///		
		/// </summary>
		/// <param name="codec_name">编码器名称</param>
		/// <param name="desire_out_video_stream_infos">期望的输出视频流信息</param>
		/// <param name="out_fmt_ctx">输出格式。编码后的视频帧将被写入该格式。</param>
		/// <param name="video_out_bitrate_in_bps">
		///		期望的视频输出比特率。设置为小于等于 0 的值表示自动。
		/// </param>
		SwsFpsEncoderPipe(
			std::string codec_name,
			IVideoStreamInfoCollection &desire_out_video_stream_infos,
			shared_ptr<OutputFormat> out_fmt_ctx,
			int64_t video_out_bitrate_in_bps
		);

	private:
		shared_ptr<IFrameConsumer> _video_encoder_pipe;
		shared_ptr<SwsPipe> _sws_pipe;
		shared_ptr<FpsAdjustPipe> _fps_adjust_pipe;

	public:
		void SendFrame(AVFrameWrapper *frame) override;
	};
}