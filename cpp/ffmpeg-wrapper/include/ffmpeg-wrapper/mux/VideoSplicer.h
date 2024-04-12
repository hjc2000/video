#pragma once
#include<ffmpeg-wrapper/demux/JoinedInputFormatDemuxDecoder.h>
#include<ffmpeg-wrapper/input-format/InputFormat.h>
#include<ffmpeg-wrapper/mux/SptsEncodeMux.h>
#include<ffmpeg-wrapper/output-format/StreamOutputFormat.h>
#include<jccpp/IDisposable.h>
#include<jccpp/container/List.h>
#include<jccpp/container/Queue.h>
#include<jccpp/stream/FileStream.h>

namespace video
{
	/// <summary>
	///		视频拼接器。将多个视频拼接成 ts。
	/// </summary>
	class VideoSplicer :public IDisposable
	{
	public:
		VideoSplicer()
		{
			Queue<std::string> file_queue;
			file_queue.Enqueue("moon.mp4");
			file_queue.Enqueue("水龙吟.ts");
			file_queue.Enqueue("越权访问.mkv");
			file_queue.Enqueue("fallen-down.ts");
			shared_ptr<JoinedInputFormatDemuxDecoder> joined_input_format_demux_decoder{ new JoinedInputFormatDemuxDecoder{} };
			joined_input_format_demux_decoder->_get_format_callback = [&]()->shared_ptr<InputFormat>
			{
				try
				{
					std::string file = file_queue.Dequeue();
					shared_ptr<InputFormat> in_fmt_ctx{ new InputFormat{ file } };
					in_fmt_ctx->DumpFormat();
					return in_fmt_ctx;
				}
				catch (std::exception &e)
				{
					cerr << e.what() << endl;
				}
				catch (...)
				{
					cerr << "发生未知异常" << endl;
				}

				return nullptr;
			};

			shared_ptr<Stream> out_fs = FileStream::CreateNewAnyway("mux_out.ts");
			shared_ptr<StreamOutputFormat> out_fmt_ctx{ new StreamOutputFormat{".ts",out_fs} };
			shared_ptr<SptsEncodeMux> spts_encode_mux{ new SptsEncodeMux{
				out_fmt_ctx,
				joined_input_format_demux_decoder->GetVideoStreamInfos(),
				"hevc_amf",
				joined_input_format_demux_decoder->GetVideoStreamInfos().Bitrate(),
				joined_input_format_demux_decoder->GetAudioStreamInfos(),
				"aac"
			} };

			joined_input_format_demux_decoder->AddVideoFrameConsumer(spts_encode_mux->VideoEncodePipe());
			joined_input_format_demux_decoder->AddAudioFrameConsumer(spts_encode_mux->AudioEncodePipe());

			CancellationTokenSource cancel_pump_source;
			TaskCompletionSignal pump_thread_exit{ false };
			std::thread([&]()
			{
				try
				{
					joined_input_format_demux_decoder->Pump(cancel_pump_source.Token());
				}
				catch (std::exception &e)
				{
					cerr << e.what() << endl;
				}
				catch (...)
				{
					cerr << "发生未知异常" << endl;
				}

				cout << "线程退出" << endl;
				pump_thread_exit.SetResult();
			}).detach();

			cin.get();
			cancel_pump_source.Cancel();
			pump_thread_exit.Wait();
		}

	private:

	};
}