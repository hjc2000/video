#pragma once
#include<FpsAdjustPipe.h>
#include<IFrameConsumer.h>
#include<OutputFormat.h>
#include<SwsFpsEncoderPipe.h>
#include<VideoStreamInfoCollection.h>

namespace video
{
	class MpegtsVideoStreamEncodeMuxPipe :public IFrameConsumer
	{
	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="out_fmt_ctx">
		///		要绑定的输出格式。一旦本对象构造完成，将会在 out_fmt_ctx 中创建新的视频流。
		/// </param>
		/// <param name="program">
		///		本对象构造完成后将会创建视频流。program 是要将视频流添加到的节目。program 来自 out_fmt_ctx。
		/// </param>
		/// <param name="out_stream_infos">输出的视频流信息。</param>
		/// <param name="codec_name">编码器名称。会先使用该名称查找编码器，如果找不到，会回退到 libx265。</param>
		/// <param name="out_bittare">传入小于等于 0 的数表示使用默认值。</param>
		/// <param name="pid">传入小于等于 0 的数表示使用默认值。</param>
		MpegtsVideoStreamEncodeMuxPipe(
			shared_ptr<OutputFormat> out_fmt_ctx,
			AVProgramWrapper program,
			IVideoStreamInfoCollection &out_stream_infos,
			std::string codec_name,
			int64_t out_bittare,
			int pid
		)
		{
			_out_fmt_ctx = out_fmt_ctx;
			_program = program;
			_out_stream_infos = out_stream_infos;
			_codec_name = codec_name;
			_out_bitrate = out_bittare;
			_pid = pid;

			_out_stream_infos.set_time_base(AVRational(1, 90000));
			InitEncodePipe();
			_out_fmt_ctx->AddStreamToProgram(_program->id, _encode_pipe->NewVideoStream().Index());
		}

	private:
		shared_ptr<OutputFormat> _out_fmt_ctx;
		AVProgramWrapper _program{};
		shared_ptr<SwsFpsEncoderPipe> _encode_pipe;
		VideoStreamInfoCollection _out_stream_infos{};
		std::string _codec_name;
		int64_t _out_bitrate = -1;
		int _pid = -1;

		void InitEncodePipe();

	public:
		void SendFrame(AVFrameWrapper *frame) override
		{
			_encode_pipe->SendFrame(frame);
		}
	};
}
