#include<FFmpeg.h>

static void encode(FFmpeg::AVCodecContext enc_ctx, FFmpeg::AVFrame frame, fstream& outfile)
{
	enc_ctx.avcodec_send_frame(frame);
	FFmpeg::AVPacket pkt;
	while (1)
	{
		try
		{
			enc_ctx.avcodec_receive_packet(pkt);
			outfile.write((char*)pkt()->data, pkt()->size);
		}
		catch (int err_code)
		{
			if (err_code == AVERROR(EAGAIN) || err_code == AVERROR_EOF)
				return;
			else if (err_code < 0)
				throw err_code;
		}

		pkt.unref();
	}
}

/// <summary>
/// 尝试编码
/// </summary>
void try_encode()
{
	int i, ret, x, y;
	fstream fs{ "output.mp4", ios_base::out | ios_base::in | ios_base::trunc | ios_base::binary };
	uint8_t endcode[] = { 0, 0, 1, 0xb7 };
	const char* filename = "output.mp4";

	/* find the mpeg1video encoder */
	FFmpeg::AVCodec codec{"mpeg1video"};
	FFmpeg::AVCodecContext codec_context{codec};

	/* put sample parameters */
	codec_context()->bit_rate = 400000;
	/* resolution must be a multiple of two */
	codec_context()->width = 352;
	codec_context()->height = 288;
	/* frames per second */
	codec_context()->time_base = AVRational{ 1, 25 };
	codec_context()->framerate = AVRational{ 25, 1 };

	/* emit one intra frame every ten frames
	* check frame pict_type before passing frame
	* to encoder, if frame()->pict_type is AV_PICTURE_TYPE_I
	* then gop_size is ignored and the output of encoder
	* will always be I frame irrespective to gop_size
	*/
	codec_context()->gop_size = 10;
	codec_context()->max_b_frames = 1;
	codec_context()->pix_fmt = AV_PIX_FMT_YUV420P;

	if (codec()->id == AV_CODEC_ID_H264)
		av_opt_set(codec_context()->priv_data, "preset", "slow", 0);

	/* open it */
	codec_context.open_codec();

	FFmpeg::AVFrame frame;
	frame()->format = codec_context()->pix_fmt;
	frame()->width = codec_context()->width;
	frame()->height = codec_context()->height;
	frame.av_frame_get_buffer(0);
	/* encode 1 second of video */
	for (i = 0; i < 25; i++)
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
		frame.av_frame_make_writable();

		/* Prepare a dummy image.
		In real code, this is where you would have your own logic for
		filling the frame. FFmpeg does not care what you put in the
		frame.
		*/
		/* Y */
		for (y = 0; y < codec_context()->height; y++)
		{
			for (x = 0; x < codec_context()->width; x++)
			{
				frame()->data[0][y * frame()->linesize[0] + x] = x + y + i * 3;
			}
		}

		/* Cb and Cr */
		for (y = 0; y < codec_context()->height / 2; y++)
		{
			for (x = 0; x < codec_context()->width / 2; x++)
			{
				frame()->data[1][y * frame()->linesize[1] + x] = 128 + y + i * 2;
				frame()->data[2][y * frame()->linesize[2] + x] = 64 + x + i * 5;
			}
		}

		frame()->pts = i;

		/* encode the image */
		encode(codec_context, frame, fs);
	}

	/* flush the encoder */
	encode(codec_context, nullptr, fs);

	/* Add sequence end code to have a real MPEG file.
	It makes only sense because this tiny examples writes packets
	directly. This is called "elementary stream" and only works for some
	codecs. To create a valid file, you usually need to write packets
	into a proper file format or protocol; see mux.c.
	*/
	if (codec()->id == AV_CODEC_ID_MPEG1VIDEO || codec()->id == AV_CODEC_ID_MPEG2VIDEO)
		fs.write((char*)endcode, sizeof(endcode));

	fs.flush();
	fs.close();
}
