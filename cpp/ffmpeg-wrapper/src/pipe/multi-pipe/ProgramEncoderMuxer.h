#pragma once
#include<AVDictionaryWrapper.h>
#include<AVStreamWrapper.h>
#include<AudioFrameInfoCollection.h>
#include<AudioStreamInfoCollection.h>
#include<CustomOutputFormatContext.h>
#include<DateTime.h>
#include<DecoderPipe.h>
#include<FileOutputFormatContext.h>
#include<FileStream.h>
#include<FpsAdjustPipe.h>
#include<InputFormatContext.h>
#include<MpegtsAudioStreamEncodeMuxPipe.h>
#include<MpegtsVideoStreamEncodeMuxPipe.h>
#include<PacketPump.h>
#include<ProgramDecoder.h>
#include<SwrEncoderPipe.h>
#include<SwsFpsEncoderPipe.h>

namespace video
{
	/// <summary>
	///		节目的编码，封装器。
	/// 
	///		* 这里的节目指的是 mpegts 的节目。每一个本类对象都要绑定一个节目。
	///		* 构造本类需要绑定一个 OutputFormat 和一个 AVProgramWrapper。其中，OutputFormat 肯定是
	///		  用户在构造函数中传进来的，至于 AVProgramWrapper，可以是用户创建好后传进来，让本类对象与之绑定，
	///		  也可以不在构造函数中传进来，这是本类对象就会自己创建一个节目来绑定。
	/// </summary>
	class ProgramEncoderMuxer
	{
	public:
		/// <summary>
		///		会在 out_fmt_ctx 中创建节目，然后对 out_fmt_ctx 和这个节目进行操作。
		/// </summary>
		/// <param name="out_fmt_ctx"></param>
		ProgramEncoderMuxer(shared_ptr<OutputFormat> out_fmt_ctx)
		{
			_out_fmt_ctx = out_fmt_ctx;
			_program = _out_fmt_ctx->CreateNewProgram();
			_program.set_service_name("ProgramEncoderMuxer");
			_program.set_service_provider("ProgramEncoderMuxer");
		}

		/// <summary>
		///		在 out_fmt_ctx 和它已有的节目 program 上进行操作。
		/// </summary>
		/// <param name="out_fmt_ctx"></param>
		/// <param name="program"></param>
		ProgramEncoderMuxer(shared_ptr<OutputFormat> out_fmt_ctx, AVProgramWrapper &program)
		{
			_out_fmt_ctx = out_fmt_ctx;
			_program = program;
		}

	private:
		shared_ptr<OutputFormat> _out_fmt_ctx;
		AVProgramWrapper _program{};

	public:
		/// <summary>
		///		在输出格式中创建新的视频流，并返回用于写入帧的编码管道。
		/// </summary>
		/// <param name="out_stream_infos"></param>
		/// <param name="codec_name"></param>
		/// <param name="out_bittare">期望的输出比特率。输入 -1 表示自动。</param>
		/// <param name="out_pid">期望的输出 PID。输入 -1 表示自动。</param>
		/// <returns></returns>
		shared_ptr<MpegtsVideoStreamEncodeMuxPipe> AddVideoStream(
			IVideoStreamInfoCollection &out_stream_infos,
			std::string codec_name,
			int64_t out_bittare,
			int out_pid
		);

		/// <summary>
		///		在输出格式中创建新的音频流，并返回用于写入帧的编码管道。
		/// </summary>
		/// <param name="out_stream_infos"></param>
		/// <param name="codec_name"></param>
		/// <param name="out_pid">期望的输出 PID。输入 -1 表示自动。</param>
		/// <returns></returns>
		shared_ptr<MpegtsAudioStreamEncodeMuxPipe> AddAudioStream(
			IAudioStreamInfoCollection &out_stream_infos,
			std::string codec_name,
			int out_pid
		);

		/// <summary>
		///		遍历 multi_source 中的每个流，然后在本节目中创建新的流和编码管道，
		///		然后将编码管道连接到 PipeFrameSource 上。
		/// </summary>
		/// <param name="multi_source">IMultiStreamPipeFrameSource 对象</param>
		/// <param name="video_codec_name">视频编码器的名称。</param>
		/// <param name="audio_codec_name">音频编码器的名称。</param>
		/// <param name="out_audio_sample_rate">
		///		不采用 multi_source 中各个音频流的原始采样率，而是使用自定义值。可以将输出音频流转换到目标采样率上。
		///		* 输入 -1 表示使用原始的采样率。
		/// </param>
		void AddMultiStream(
			shared_ptr<IMultiStreamPipeFrameSource> multi_source,
			std::string video_codec_name,
			std::string audio_codec_name,
			int out_audio_sample_rate
		);

		/// <summary>
		///		初始化完毕后，调用此方法向格式写入头部。ts 不需要写尾部信息。
		/// </summary>
		void WriteHeader();
	};

	void test_ProgramEncoderMuxer();
}
