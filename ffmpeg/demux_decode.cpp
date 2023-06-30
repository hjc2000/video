#include<FFmpeg.h>

FFmpeg::AVCodecContext bestAudioDecodeCtx;
static int width, height;
static enum AVPixelFormat pix_fmt;
static FILE *video_dst_file = NULL;
static FILE *audio_dst_file = NULL;

static uint8_t *video_dst_data[4] = { NULL };
static int video_dst_linesize[4];
static int video_dst_bufsize;

static int video_frame_count = 0;
static int audio_frame_count = 0;

static int output_video_frame(AVFrame *frame)
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

static int decode_packet(FFmpeg::AVCodecContext decoderCtx, FFmpeg::AVPacket pkt, FFmpeg::AVFrame frame)
{
	// submit the packet to the decoder
	decoderCtx.send_packet(pkt);
	int ret = 0;

	// get all the available frames from the decoder
	while (decoderCtx.receive_frame(frame))
	{
		// write the frame data to output file
		if (decoderCtx()->codec->type == AVMEDIA_TYPE_VIDEO)
			ret = output_video_frame(frame);
		else
			ret = output_audio_frame(frame);

		frame.unref();
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int get_format_from_sample_fmt(const char **fmt,
	enum AVSampleFormat sample_fmt)
{
	int i;
	struct sample_fmt_entry
	{
		enum AVSampleFormat sample_fmt;
		const char *fmt_be, *fmt_le;
	} sample_fmt_entries[] = {
		{AV_SAMPLE_FMT_U8, "u8", "u8"},
		{AV_SAMPLE_FMT_S16, "s16be", "s16le"},
		{AV_SAMPLE_FMT_S32, "s32be", "s32le"},
		{AV_SAMPLE_FMT_FLT, "f32be", "f32le"},
		{AV_SAMPLE_FMT_DBL, "f64be", "f64le"},
	};
	*fmt = NULL;

	for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++)
	{
		struct sample_fmt_entry *entry = &sample_fmt_entries[i];
		if (sample_fmt == entry->sample_fmt)
		{
			*fmt = AV_NE(entry->fmt_be, entry->fmt_le);
			return 0;
		}
	}

	fprintf(stderr,
		"sample format %s is not supported as output format\n",
		av_get_sample_fmt_name(sample_fmt));
	return -1;
}

int demux_decode_main(const char *src_filename)
{
	// 打开输入文件
	FFmpeg::AVFormatContext inputFormatCtx;
	inputFormatCtx.open_input(src_filename);
	inputFormatCtx.find_stream_info();

	// 准备视频解码器
	FFmpeg::AVStream bestVideoStream;
	FFmpeg::AVCodec bestVideoDecodeCodec;
	FFmpeg::AVCodecContext bestVideoDecodeCtx;

	try
	{
		bestVideoStream = inputFormatCtx.find_best_stream(FFmpeg::AVMediaType::AVMEDIA_TYPE_VIDEO);
		bestVideoDecodeCodec = bestVideoStream.get_stream_codec();
		bestVideoDecodeCtx = FFmpeg::AVCodecContext::create(bestVideoDecodeCodec, bestVideoStream()->codecpar);
		bestVideoDecodeCtx.open_codec();

		video_dst_file = fopen("out_video.yuv", "wb");
		if (!video_dst_file)
			throw "无法打开视频解码输出文件";

		/* allocate image where the decoded image will be put */
		width = bestVideoDecodeCtx()->width;
		height = bestVideoDecodeCtx()->height;
		pix_fmt = bestVideoDecodeCtx()->pix_fmt;
		int size = av_image_alloc(video_dst_data, video_dst_linesize, width, height, pix_fmt, 1);
		if (size < 0)
		{
			fprintf(stderr, "Could not allocate raw video buffer\n");
			throw size;
		}

		video_dst_bufsize = size;
	}
	catch (int err_code)
	{
		cout << "查找视频流时发生异常" << endl;
		cout << FFmpeg::error_code_to_str(err_code) << endl;
	}

	// 准备音频解码器
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
			throw "无法打开音频解码输出文件";
	}
	catch (int err)
	{
		cout << "查找音频流时发生异常" << endl;
		cout << FFmpeg::error_code_to_str(err) << endl;
	}

	// 打印流信息
	inputFormatCtx.dump_format(bestVideoStream()->index, src_filename, 0);

	if (!bestAudioStream && !bestVideoStream)
		throw "找不到音频流和视频流";

	FFmpeg::AVFrame frame = FFmpeg::AVFrame::create();
	FFmpeg::AVPacket pkt;
	/* read frames from the file */

	try
	{
		int ret = -1;
		while (inputFormatCtx.read_packet(pkt))
		{
			if (pkt()->stream_index == bestVideoStream()->index)
				ret = decode_packet(bestVideoDecodeCtx, pkt, frame);
			else if (pkt()->stream_index == bestAudioStream()->index)
				ret = decode_packet(bestAudioDecodeCtx, pkt, frame);
			pkt.unref();
			if (ret < 0)
				break;
		}
	}
	catch (int err)
	{
		cout << "发生了异常" << endl;
	}

	/* flush the decoders */
	if (bestVideoDecodeCtx)
		decode_packet(bestVideoDecodeCtx, NULL, frame);
	if (bestAudioDecodeCtx)
		decode_packet(bestAudioDecodeCtx, NULL, frame);

	if (bestAudioStream)
	{
		enum AVSampleFormat sfmt = bestAudioDecodeCtx()->sample_fmt;
		int n_channels = bestAudioDecodeCtx()->ch_layout.nb_channels;
		const char *fmt;

		if (av_sample_fmt_is_planar(sfmt))
		{
			const char *packed = av_get_sample_fmt_name(sfmt);
			printf("Warning: the sample format the decoder produced is planar "
				"(%s). This example will output the first channel only.\n",
				packed ? packed : "?");
			sfmt = av_get_packed_sample_fmt(sfmt);
			n_channels = 1;
		}

		if (get_format_from_sample_fmt(&fmt, sfmt) < 0)
			goto end;
	}

end:
	if (video_dst_file)
		fclose(video_dst_file);
	if (audio_dst_file)
		fclose(audio_dst_file);
	av_free(video_dst_data[0]);
}
