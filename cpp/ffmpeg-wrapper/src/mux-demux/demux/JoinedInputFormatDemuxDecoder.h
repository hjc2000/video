#pragma once
#include<AVStreamInfoCollection.h>
#include<CancellationTokenSource.h>
#include<DecoderPipe.h>
#include<IDisposable.h>
#include<InputFormatContext.h>
#include<functional>
#include<memory>

namespace video
{
	/// <summary>
	///		将输入格式拼接起来，进行解封装、解码。一个输入格式结束后会继续获取
	///		下一个输入格式。最终输出来自不同输入格式解封装、解码后的视频帧、音频帧。
	/// </summary>
	class JoinedInputFormatDemuxDecoder :
		public IDisposable
	{
	public:
		void Dispose() override
		{

		}

	private:
		shared_ptr<InputFormatContext> _current_intput_format;

		AVStreamInfoCollection _video_stream_infos;
		shared_ptr<DecoderPipe> _video_decode_pipe;

		AVStreamInfoCollection _audio_stream_infos;
		shared_ptr<DecoderPipe> _audio_decode_pipe;

		void OpenInputIfNull();

	public:
		/// <summary>
		///		当需要输入封装时就会触发此回调。
		///		* 回调函数返回 InputFormatContext 对象则视频流继续。
		///		* 回调函数返回空指针则结束视频流。
		/// </summary>
		std::function<shared_ptr<InputFormatContext>()> _get_format_callback;

		void Pump(shared_ptr<CancellationToken> cancel_pump);
	};
}