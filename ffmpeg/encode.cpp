#include<FFmpeg.h>

static void encode(FFmpeg::AVCodecContext enc_ctx, FFmpeg::AVFrame frame, fstream &outfile)
{
	enc_ctx.avcodec_send_frame(frame);
	FFmpeg::AVPacket pkt;
	while (!enc_ctx.avcodec_receive_packet(pkt))
	{
		outfile.write((char *)pkt()->data, pkt()->size);
		pkt.unref();
	}
}

/// <summary>
/// 尝试编码
/// </summary>
void encode_main()
{
	fstream outputFS{ "output.mp4", ios_base::out | ios_base::in | ios_base::trunc | ios_base::binary };
	uint8_t endcode[] = { 0, 0, 1, 0xb7 };
	const char *outputFileName = "output.mp4";

	/* find the mpeg1video encoder */
	FFmpeg::AVCodec encoder = FFmpeg::AVCodec::find_encoder_by_name("mpeg1video");
	FFmpeg::AVCodecContext encoderCtx = FFmpeg::AVCodecContext::create(encoder);

	#pragma region 手动设置编码器上下文的参数
	/* put sample parameters */
	encoderCtx()->bit_rate = 400000;
	/* resolution must be a multiple of two */
	encoderCtx()->width = 352;
	encoderCtx()->height = 288;
	/* frames per second */
	encoderCtx()->time_base = AVRational{ 1, 25 };
	encoderCtx()->framerate = AVRational{ 25, 1 };

	/* emit one intra frame every ten frames
	* check frame pict_type before passing frame
	* to encoder, if frame()->pict_type is AV_PICTURE_TYPE_I
	* then gop_size is ignored and the output of encoder
	* will always be I frame irrespective to gop_size
	*/
	encoderCtx()->gop_size = 10;
	encoderCtx()->max_b_frames = 1;
	encoderCtx()->pix_fmt = AV_PIX_FMT_YUV420P;
	#pragma endregion

	if (encoder()->id == AV_CODEC_ID_H264)
	{
		av_opt_set(encoderCtx()->priv_data, "preset", "slow", 0);
	}

	/* open it */
	encoderCtx.open();

	FFmpeg::AVFrame frame;
	frame = FFmpeg::AVFrame::create();
	frame()->format = encoderCtx()->pix_fmt;
	frame()->width = encoderCtx()->width;
	frame()->height = encoderCtx()->height;
	frame.av_frame_get_buffer(0);
	/* encode 1 second of video */
	for (int i = 0; i < 25; i++)
	{
		/* Make sure the frame data is writable.
		On the first round, the frame is fresh from av_frame_get_buffer()
		and therefore we know it is writable.
		But on the next rounds, encode() will have called
		avcodec_send_frame(), and the codec may have kept a reference to
		the frame in its internal structures, that makes the frame
		unwritable.
		av_frame_make_writable() checks that and allocates a new buffer
		for the frame only if necessary.
		*/
		frame.make_writable();

		#pragma region 在数组中生成视频裸流
		/* Prepare a dummy image.
		In real code, this is where you would have your own logic for
		filling the frame. FFmpeg does not care what you put in the
		frame.
		*/
		/* Y */
		for (int y = 0; y < encoderCtx()->height; y++)
		{
			for (int x = 0; x < encoderCtx()->width; x++)
			{
				frame()->data[0][y * frame()->linesize[0] + x] = x + y + i * 3;
			}
		}

		/* Cb and Cr */
		for (int y = 0; y < encoderCtx()->height / 2; y++)
		{
			for (int x = 0; x < encoderCtx()->width / 2; x++)
			{
				frame()->data[1][y * frame()->linesize[1] + x] = 128 + y + i * 2;
				frame()->data[2][y * frame()->linesize[2] + x] = 64 + x + i * 5;
			}
		}
		#pragma endregion

		frame()->pts = i;

		/* encode the image */
		encode(encoderCtx, frame, outputFS);
	}

	/* flush the encoder */
	encode(encoderCtx, nullptr, outputFS);

	/* Add sequence end code to have a real MPEG file.
	It makes only sense because this tiny examples writes packets
	directly. This is called "elementary stream" and only works for some
	codecs. To create a valid file, you usually need to write packets
	into a proper file format or protocol; see mux.c.
	*/
	if (encoder()->id == AV_CODEC_ID_MPEG1VIDEO || encoder()->id == AV_CODEC_ID_MPEG2VIDEO)
	{
		outputFS.write((char *)endcode, sizeof(endcode));
	}

	outputFS.flush();
	outputFS.close();
}
