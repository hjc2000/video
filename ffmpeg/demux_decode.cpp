#include<FFmpeg.h>
using FFmpeg::Exception;

FFmpeg::AVCodecContext bestAudioDecodeCtx;
static int width, height;
static enum AVPixelFormat pix_fmt;
static FILE *video_dst_file = NULL;
static FILE *audio_dst_file = NULL;

static int video_dst_linesize[4];
static int video_dst_bufsize;

static int video_frame_count = 0;
static int audio_frame_count = 0;

static int output_video_frame(AVFrame *frame, uint8_t **video_dst_data)
{
	if (frame->width != width || frame->height != height ||
		frame->format != pix_fmt)
	{
		/* To handle this change, one could call av_image_alloc again and
		 * decode the following frames into another rawvideo file. */
		fprintf(stderr, "Error: Width, height and pixel format have to be "
			"constant in a rawvideo file, but the width, height or "
			"pixel format of the input video changed:\n"
			"old: width = %d, height = %d, format = %s\n"
			"new: width = %d, height = %d, format = %s\n",
			width, height, av_get_pix_fmt_name(pix_fmt),
			frame->width, frame->height,
			av_get_pix_fmt_name((AVPixelFormat)frame->format));
		return -1;
	}

	printf("video_frame n:%d\n",
		video_frame_count++);

	/* copy decoded frame to destination buffer:
	 * this is required since rawvideo expects non aligned data */
	av_image_copy(video_dst_data, video_dst_linesize,
		(const uint8_t **)(frame->data), frame->linesize,
		pix_fmt, width, height);

	/* write to rawvideo file */
	fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file);
	return 0;
}

static int output_audio_frame(AVFrame *frame)
{
	char err_buff[32];
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
	fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);

	return 0;
}

int demux_decode_main(const char *src_filename)
{
	uint8_t *video_dst_data[4] = { NULL };

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

		video_dst_file = fopen("out_video.yuv", "wb");
		if (!video_dst_file)
			throw Exception("无法打开视频解码输出文件");

		/* allocate image where the decoded image will be put */
		width = bestVideoDecodeCtx()->width;
		height = bestVideoDecodeCtx()->height;
		pix_fmt = bestVideoDecodeCtx()->pix_fmt;
		int size = av_image_alloc(video_dst_data, video_dst_linesize, width, height, pix_fmt, 1);
		if (size < 0)
		{
			throw Exception("Could not allocate raw video buffer", size);
		}

		video_dst_bufsize = size;
	}
	catch (Exception e)
	{
		cout << e.what() << endl;
	}
	#pragma endregion

	#pragma region 准备音频解码器
	FFmpeg::AVStream bestAudioStream;
	FFmpeg::AVCodec bestAudioDecodeCodec;

	try
	{
		bestAudioStream = inputFormatCtx.find_best_stream(FFmpeg::AVMediaType::AVMEDIA_TYPE_AUDIO);
		bestAudioDecodeCodec = bestAudioStream.get_stream_codec();
		bestAudioDecodeCtx = FFmpeg::AVCodecContext::create(bestAudioDecodeCodec, bestAudioStream()->codecpar);
		bestAudioDecodeCtx.open_codec();
		audio_dst_file = fopen("out_audio.pcm", "wb");
		if (!audio_dst_file)
			throw Exception("无法打开音频解码输出文件");
	}
	catch (Exception e)
	{
		cout << "查找音频流时发生异常：" << e.what() << endl;
	}
	#pragma endregion

	// 打印流信息
	inputFormatCtx.dump_format(bestVideoStream()->index, src_filename, 0);

	if (!bestAudioStream && !bestVideoStream)
		throw Exception("找不到音频流和视频流");

	FFmpeg::AVFrame frame = FFmpeg::AVFrame::create();
	FFmpeg::AVPacket pkt;
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
				int	ret = output_video_frame(frame, video_dst_data);
				frame.unref();
				if (ret < 0)
					throw Exception("接收解码后的视频帧后执行 output_video_frame 失败", ret);
			}
		}
		else if (pkt()->stream_index == bestAudioStream()->index)
		{
			// 如果读取到的包是音频流的包
			bestAudioDecodeCtx.send_packet(pkt);
			while (!bestAudioDecodeCtx.receive_frame(frame))
			{
				int	ret = output_audio_frame(frame);
				frame.unref();
				if (ret < 0)
					throw Exception("接收解码后的音频帧后执行 output_video_frame 失败", ret);
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
			int	ret = output_video_frame(frame, video_dst_data);
			frame.unref();
			if (ret < 0)
				throw Exception("刷新视频解码器缓冲区时异常：", ret);
		}
	}

	if (bestAudioDecodeCtx)
	{
		bestAudioDecodeCtx.send_packet(nullptr);
		// get all the available frames from the decoder
		while (!bestAudioDecodeCtx.receive_frame(frame))
		{
			int	ret = output_audio_frame(frame);
			frame.unref();
			if (ret < 0)
				throw Exception("刷新音频解码器缓冲区时异常：", ret);
		}
	}

end:
	if (video_dst_file)
		fclose(video_dst_file);
	if (audio_dst_file)
		fclose(audio_dst_file);
	av_free(video_dst_data[0]);
}
