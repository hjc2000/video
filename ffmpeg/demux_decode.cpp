#include<FFmpeg.h>
#include<fstream>
using std::fstream;
using FFmpeg::Exception;

void output_audio_frame(AVFrame *frame, fstream &audio_dst_file, FFmpeg::AVCodecContext bestAudioDecodeCtx)
{
	char err_buff[32];
	static int audio_frame_count = 0;
	size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->format);
	printf("audio_frame n:%d nb_samples:%d pts:%s\n",
		audio_frame_count++, frame->nb_samples,
		av_ts_make_time_string(err_buff, frame->pts, &(bestAudioDecodeCtx()->time_base)));

	/* Write the raw audio data samples of the first plane. This works
	 * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
	 * most audio decoders output planar audio, which uses a separate
	 * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
	 * In other words, this code will write only the first audio channel
	 * in these cases.
	 * You should use libswresample or libavfilter to convert the frame
	 * to packed data. */
	audio_dst_file.write((char *)(frame->extended_data[0]), unpadded_linesize);
}

int demux_decode_main(const char *src_filename)
{
	// 准备音频解码输出文件、视频解码输出文件的文件流
	fstream video_dst_file{ "out_video.yuv", ios_base::out | ios_base::in | ios_base::trunc | ios_base::binary };
	fstream audio_dst_file{ "out_audio.pcm", ios_base::out | ios_base::in | ios_base::trunc | ios_base::binary };

	// 打开输入格式
	FFmpeg::AVFormatContext inputFormatCtx;
	inputFormatCtx.open_input(src_filename);
	inputFormatCtx.find_stream_info();

	#pragma region 准备视频解码器
	// 最好的视频流
	FFmpeg::AVStream bestVideoStream;
	// 用来解码最好的视频流的解码器
	FFmpeg::AVCodec bestVideoDecoder;
	// 解码器上下文
	FFmpeg::AVCodecContext bestVideoDecoderCtx;

	try
	{
		// 查找最好的视频流
		bestVideoStream = inputFormatCtx.find_best_stream(FFmpeg::AVMediaType::AVMEDIA_TYPE_VIDEO);
		// 获取最好的视频流的解码器
		bestVideoDecoder = bestVideoStream.get_stream_codec();
		// 使用解码器创建解码器上下文
		bestVideoDecoderCtx = FFmpeg::AVCodecContext::create(bestVideoDecoder, bestVideoStream()->codecpar, true);
	}
	catch (Exception e)
	{
		cout << e << endl;
	}
	#pragma endregion

	#pragma region 准备音频解码器
	FFmpeg::AVStream bestAudioStream;
	FFmpeg::AVCodec bestAudioDecoder;
	FFmpeg::AVCodecContext bestAudioDecoderCtx;

	try
	{
		bestAudioStream = inputFormatCtx.find_best_stream(FFmpeg::AVMediaType::AVMEDIA_TYPE_AUDIO);
		bestAudioDecoder = bestAudioStream.get_stream_codec();
		bestAudioDecoderCtx = FFmpeg::AVCodecContext::create(bestAudioDecoder, bestAudioStream()->codecpar, true);
	}
	catch (Exception e)
	{
		cout << e << endl;
	}
	#pragma endregion

	// 打印流信息
	inputFormatCtx.dump_format(bestVideoStream()->index, src_filename, 0);

	if (!bestAudioStream && !bestVideoStream)
	{
		throw Exception("找不到音频流和视频流");
	}

	FFmpeg::AVFrame frame = FFmpeg::AVFrame::create();
	FFmpeg::AVPacket pkt;

	/* uint8_t * 指针变量的数组，数组中每个单元格储存一个 uint8_t * 指针。
	* 每个指针都指向一个 uint8_t 数组的首地址
	* 这么看来 video_dst_datas 是一个每一行可能不等长的二维数组。第一行储存 Y 数组，
	  第二行储存 U 数组，第三行储存 V 数组。第四行是预留的用来扩展的。*/
	uint8_t *video_dst_datas[4] = { nullptr };

	/* 上面说了 video_dst_datas 是一个每一行可能不等长的二维数组，这个变量是用来储存每一行
	的长度的 */
	int video_dst_datas_linesizes[4];

	/* 为 video_dst_datas 在堆中分配内存。
	整个二维数组在堆中是连续的，然后将其中的特定位置的指针赋值给 video_dst_datas 中的每个单元格*/
	int size = av_image_alloc(video_dst_datas, video_dst_datas_linesizes,
		bestVideoDecoderCtx()->width, bestVideoDecoderCtx()->height,
		bestVideoDecoderCtx()->pix_fmt, 1);

	if (size < 0)
	{
		throw Exception("Could not allocate raw video buffer", size);
	}

	// 在循环中读取格式中的包
	while (!inputFormatCtx.read_packet(pkt))
	{
		// 如果读到的包是视频流的包
		if (pkt()->stream_index == bestVideoStream()->index)
		{
			// 向视频解码器发送包
			bestVideoDecoderCtx.send_packet(pkt);
			while (!bestVideoDecoderCtx.receive_frame(frame))
			{
				static int video_frame_count = 0;
				printf("video_frame n:%d\n", video_frame_count++);
				// 将解码帧复制到目标缓冲区：这是必需的，因为rawvideo需要不对齐的数据
				frame.copy_image_to_arr(video_dst_datas, video_dst_datas_linesizes);
				/* video_dst_datas 是一个长度为 4 的指针数组，里面储存着 4 个指针。前面说过，
				video_dst_datas 也可以当作是一个二维数组。只不过每一行的长度可能不同。
				* video_dst_datas[0] 是这个指针数组里的第一个指针，指向这个二维数组的第一行的头部。
				因为 video_dst_datas 这个二维数组的所有单元格在内存上是连续的（一行接着一行，
				第二行的位置紧跟在第二行后面），所以 video_dst_datas[0] 内的指针指向的是整个缓冲区的头部，
				而 size 是整个缓冲区的大小（每一行的长度相加）。
				* 下面的这句就相当于在循环中遍历 video_dst_datas 中的每一行并写入文件。*/
				video_dst_file.write((char *)video_dst_datas[0], size);
				frame.unref();
			}
		}
		else if (pkt()->stream_index == bestAudioStream()->index)
		{
			// 如果读取到的包是音频流的包
			bestAudioDecoderCtx.send_packet(pkt);
			while (!bestAudioDecoderCtx.receive_frame(frame))
			{
				output_audio_frame(frame, audio_dst_file, bestAudioDecoderCtx);
				frame.unref();
			}
		}

		pkt.unref();
	}

	/* flush the decoders */
	if (bestVideoDecoderCtx)
	{
		bestVideoDecoderCtx.send_packet(nullptr);
		// 在循环中读取解码后的帧
		while (!bestVideoDecoderCtx.receive_frame(frame))
		{
			// 将解码帧复制到目标缓冲区：这是必需的，因为rawvideo需要不对齐的数据
			frame.copy_image_to_arr(video_dst_datas, video_dst_datas_linesizes);
			video_dst_file.write((char *)video_dst_datas[0], size);
			frame.unref();
		}
	}

	if (bestAudioDecoderCtx)
	{
		bestAudioDecoderCtx.send_packet(nullptr);
		// get all the available frames from the decoder
		while (!bestAudioDecoderCtx.receive_frame(frame))
		{
			output_audio_frame(frame, audio_dst_file, bestAudioDecoderCtx);
			frame.unref();
		}
	}

	if (video_dst_file)
	{
		video_dst_file.flush();
		video_dst_file.close();
	}

	if (audio_dst_file)
	{
		audio_dst_file.flush();
		audio_dst_file.close();
	}

	av_free(video_dst_datas[0]);
}
