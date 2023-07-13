#include<FFmpeg.h>
#include<fstream>

using std::fstream;
using FFmpeg::Exception;

void output_audio_frame(shared_ptr<FFmpeg::AVFrame> frame, fstream &audio_dst_file, shared_ptr<FFmpeg::AVCodecContext> bestAudioDecodeCtx)
{
	size_t unpadded_linesize = frame->w->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->w->format);

	/* Write the raw audio data samples of the first plane. This works
	 * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
	 * most audio decoders output planar audio, which uses a separate
	 * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
	 * In other words, this code will write only the first audio channel
	 * in these cases.
	 * You should use libswresample or libavfilter to convert the spFrame
	 * to packed data. */
	audio_dst_file.write((char *)(frame->w->extended_data[0]), unpadded_linesize);
}

int demux_decode_main(const char *src_filename)
{
	::avformat_network_init();
	// 准备音频解码输出文件、视频解码输出文件的文件流
	fstream video_out_fs{ "out_video.yuv", ios_base::out | ios_base::in | ios_base::trunc | ios_base::binary };
	fstream audio_dst_file{ "out_audio.pcm", ios_base::out | ios_base::in | ios_base::trunc | ios_base::binary };

	// 打开输入格式
	shared_ptr<FFmpeg::AVFormatContext> spInputFormatCtx = FFmpeg::AVFormatContext::open_input(src_filename);
	spInputFormatCtx->find_stream_info();

	#pragma region 准备视频解码器
	// 最好的视频流
	shared_ptr<FFmpeg::AVStream> spBestVideoStream = nullptr;
	// 用来解码最好的视频流的解码器
	shared_ptr<FFmpeg::AVCodec> bestVideoDecoder;
	// 解码器上下文
	FFmpeg::AVCodecContext bestVideoDecoderCtx;

	try
	{
		// 查找最好的视频流
		spBestVideoStream = spInputFormatCtx->find_best_stream(FFmpeg::AVMediaType::AVMEDIA_TYPE_VIDEO);
		// 获取最好的视频流的解码器
		bestVideoDecoder = spBestVideoStream->get_stream_codec();
		// 使用解码器创建解码器上下文
		bestVideoDecoderCtx = FFmpeg::AVCodecContext{ bestVideoDecoder, (*spBestVideoStream)()->codecpar, true };
	}
	catch (Exception e)
	{
		cout << e << endl;
	}
	#pragma endregion

	#pragma region 准备音频解码器
	shared_ptr<FFmpeg::AVStream> spBestAudioStream;
	shared_ptr<FFmpeg::AVCodec> bestAudioDecoder;
	shared_ptr<FFmpeg::AVCodecContext> spBestAudioDecoderCtx;

	try
	{
		spBestAudioStream = spInputFormatCtx->find_best_stream(FFmpeg::AVMediaType::AVMEDIA_TYPE_AUDIO);
		bestAudioDecoder = spBestAudioStream->get_stream_codec();
		spBestAudioDecoderCtx = shared_ptr<FFmpeg::AVCodecContext>(new FFmpeg::AVCodecContext{ bestAudioDecoder, (*spBestAudioStream)()->codecpar, true });
	}
	catch (Exception e)
	{
		cout << e << endl;
	}
	#pragma endregion

	// 打印流信息
	spInputFormatCtx->dump_format((*spBestVideoStream)()->index, src_filename, 0);

	if (!spBestAudioStream && !spBestVideoStream)
	{
		throw Exception("找不到音频流和视频流");
	}

	shared_ptr<FFmpeg::AVFrame> spFrame{new FFmpeg::AVFrame{}};
	shared_ptr<FFmpeg::AVPacket> spPacket{new FFmpeg::AVPacket{}};
	FFmpeg::ImageBuffer buffer{bestVideoDecoderCtx, 1};

	// 在循环中读取格式中的包
	while (!spInputFormatCtx->read_packet(spPacket))
	{
		// 如果读到的包是视频流的包
		if (spPacket->w->stream_index == spBestVideoStream->w->index)
		{
			// 向视频解码器发送包
			bestVideoDecoderCtx.send_packet(spPacket);
			while (!bestVideoDecoderCtx.receive_frame(spFrame))
			{
				static int video_frame_count = 0;
				printf("video_frame n:%d\n", video_frame_count++);
				// 将解码帧复制到目标缓冲区：这是必需的，因为rawvideo需要不对齐的数据
				spFrame->copy_image_to_buffer(buffer);
				/* video_dst_datas 是一个长度为 4 的指针数组，里面储存着 4 个指针。前面说过，
				video_dst_datas 也可以当作是一个二维数组。只不过每一行的长度可能不同。
				* video_dst_datas[0] 是这个指针数组里的第一个指针，指向这个二维数组的第一行的头部。
				因为 video_dst_datas 这个二维数组的所有单元格在内存上是连续的（一行接着一行，
				第二行的位置紧跟在第二行后面），所以 video_dst_datas[0] 内的指针指向的是整个缓冲区的头部，
				而 size 是整个缓冲区的大小（每一行的长度相加）。
				* 下面的这句就相当于在循环中遍历 video_dst_datas 中的每一行并写入文件。*/
				video_out_fs << buffer;
				spFrame->unref();
			}
		}
		else if (spPacket->w->stream_index == (*spBestAudioStream)()->index)
		{
			// 如果读取到的包是音频流的包
			spBestAudioDecoderCtx->send_packet(spPacket);
			while (!spBestAudioDecoderCtx->receive_frame(spFrame))
			{
				output_audio_frame(spFrame, audio_dst_file, spBestAudioDecoderCtx);
				spFrame->unref();
			}
		}

		spPacket->unref();
	}

	/* flush the decoders */
	if (bestVideoDecoderCtx)
	{
		bestVideoDecoderCtx.send_packet(nullptr);
		// 在循环中读取解码后的帧
		while (!bestVideoDecoderCtx.receive_frame(spFrame))
		{
			// 将解码帧复制到目标缓冲区：这是必需的，因为rawvideo需要不对齐的数据
			spFrame->copy_image_to_buffer(buffer);
			video_out_fs << buffer;
			spFrame->unref();
		}
	}

	if (spBestAudioDecoderCtx)
	{
		spBestAudioDecoderCtx->send_packet(nullptr);
		// get all the available frames from the decoder
		while (!spBestAudioDecoderCtx->receive_frame(spFrame))
		{
			output_audio_frame(spFrame, audio_dst_file, spBestAudioDecoderCtx);
			spFrame->unref();
		}
	}

	if (video_out_fs)
	{
		video_out_fs.flush();
		video_out_fs.close();
	}

	if (audio_dst_file)
	{
		audio_dst_file.flush();
		audio_dst_file.close();
	}

	::avformat_network_deinit();
}
