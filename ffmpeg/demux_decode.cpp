#include<FFmpeg.h>
#include<fstream>
using std::fstream;
using FFmpeg::Exception;

static int video_dst_linesize[4];
static int video_dst_bufsize;

void output_video_frame(FFmpeg::AVFrame frame, uint8_t **video_dst_data, fstream &video_dst_file)
{
	static int video_frame_count = 0;
	printf("video_frame n:%d\n", video_frame_count++);
	// 将解码帧复制到目标缓冲区：这是必需的，因为rawvideo需要不对齐的数据
	frame.copy_image_to(video_dst_data, video_dst_linesize);

	/* write to rawvideo file */
	video_dst_file.write((char *)video_dst_data[0], video_dst_bufsize);
}

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
	uint8_t *video_dst_data[4] = { nullptr };
	fstream video_dst_file{ "out_video.yuv", ios_base::out | ios_base::in | ios_base::trunc | ios_base::binary };
	fstream audio_dst_file{ "out_audio.pcm", ios_base::out | ios_base::in | ios_base::trunc | ios_base::binary };

	// 打开输入文件
	FFmpeg::AVFormatContext inputFormatCtx;
	inputFormatCtx.open_input(src_filename);
	inputFormatCtx.find_stream_info();

	#pragma region 准备视频解码器
	// 最好的视频流
	FFmpeg::AVStream bestVideoStream;
	// 用来解码最好的视频流的解码器
	FFmpeg::AVCodec bestVideoDecodeCodec;
	// 解码器上下文
	FFmpeg::AVCodecContext bestVideoDecodeCtx;

	try
	{
		bestVideoStream = inputFormatCtx.find_best_stream(FFmpeg::AVMediaType::AVMEDIA_TYPE_VIDEO);
		bestVideoDecodeCodec = bestVideoStream.get_stream_codec();
		bestVideoDecodeCtx = FFmpeg::AVCodecContext::create(bestVideoDecodeCodec, bestVideoStream()->codecpar);
		bestVideoDecodeCtx.open_codec();
	}
	catch (Exception e)
	{
		cout << e << endl;
	}
	#pragma endregion

	#pragma region 准备音频解码器
	FFmpeg::AVStream bestAudioStream;
	FFmpeg::AVCodec bestAudioDecodeCodec;
	FFmpeg::AVCodecContext bestAudioDecodeCtx;

	try
	{
		bestAudioStream = inputFormatCtx.find_best_stream(FFmpeg::AVMediaType::AVMEDIA_TYPE_AUDIO);
		bestAudioDecodeCodec = bestAudioStream.get_stream_codec();
		bestAudioDecodeCtx = FFmpeg::AVCodecContext::create(bestAudioDecodeCodec, bestAudioStream()->codecpar);
		bestAudioDecodeCtx.open_codec();
	}
	catch (Exception e)
	{
		cout << e << endl;
	}
	#pragma endregion

	// 打印流信息
	inputFormatCtx.dump_format(bestVideoStream()->index, src_filename, 0);

	if (!bestAudioStream && !bestVideoStream)
		throw Exception("找不到音频流和视频流");

	FFmpeg::AVFrame frame = FFmpeg::AVFrame::create();
	FFmpeg::AVPacket pkt;

	int size = av_image_alloc(video_dst_data, video_dst_linesize,
		bestVideoDecodeCtx()->width, bestVideoDecodeCtx()->height, bestVideoDecodeCtx()->pix_fmt, 1);
	if (size < 0)
		throw Exception("Could not allocate raw video buffer", size);

	video_dst_bufsize = size;

	// 在循环中读取格式中的包
	while (inputFormatCtx.read_packet(pkt))
	{
		// 如果读到的包是视频流的包
		if (pkt()->stream_index == bestVideoStream()->index)
		{
			// 向视频解码器发送包
			bestVideoDecodeCtx.send_packet(pkt);
			while (!bestVideoDecodeCtx.receive_frame(frame))
			{
				output_video_frame(frame, video_dst_data, video_dst_file);
				frame.unref();
			}
		}
		else if (pkt()->stream_index == bestAudioStream()->index)
		{
			// 如果读取到的包是音频流的包
			bestAudioDecodeCtx.send_packet(pkt);
			while (!bestAudioDecodeCtx.receive_frame(frame))
			{
				output_audio_frame(frame, audio_dst_file, bestAudioDecodeCtx);
				frame.unref();
			}
		}
		pkt.unref();
	}

	/* flush the decoders */
	if (bestVideoDecodeCtx)
	{
		bestVideoDecodeCtx.send_packet(nullptr);
		// 在循环中读取解码后的帧
		while (!bestVideoDecodeCtx.receive_frame(frame))
		{
			// write the frame data to output file
			output_video_frame(frame, video_dst_data, video_dst_file);
			frame.unref();
		}
	}

	if (bestAudioDecodeCtx)
	{
		bestAudioDecodeCtx.send_packet(nullptr);
		// get all the available frames from the decoder
		while (!bestAudioDecodeCtx.receive_frame(frame))
		{
			output_audio_frame(frame, audio_dst_file, bestAudioDecodeCtx);
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

	av_free(video_dst_data[0]);
}
