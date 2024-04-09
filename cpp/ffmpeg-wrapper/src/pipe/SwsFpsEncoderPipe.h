#pragma once
#include<EncoderPipe.h>
#include<FpsAdjustPipe.h>
#include<ffmpeg-wrapper/format/output-format/OutputFormat.h>
#include<SwsPipe.h>
#include<VideoStreamInfoCollection.h>

namespace video
{
	class SwsFpsEncoderPipe :
		public IFrameConsumer
	{
	public:
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
		shared_ptr<EncoderPipe> _video_encoder_pipe;
		shared_ptr<SwsPipe> _sws_pipe;
		shared_ptr<FpsAdjustPipe> _fps_adjust_pipe;

	public:
		AVStreamWrapper NewVideoStream()
		{
			return _video_encoder_pipe->Stream();
		}

		void SendFrame(AVFrameWrapper *frame) override;
	};
}