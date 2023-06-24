#define _CRT_SECURE_NO_WARNINGS
#include <include_ffmpeg.h>
#include <AVDictionary.h>
#include <AVFormatContext.h>
#include <AVError.h>
#include <AVUtil.h>
#include <AVStream.h>
#include <AVPacket.h>
#include <AVCodec.h>
#include <AVCodecContext.h>
#include<AVFrame.h>
#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::endl;
using std::fstream;
using std::ios_base;
using std::string;

static void encode(FFmpeg::AVCodecContext enc_ctx, FFmpeg::AVFrame frame, fstream& outfile)
{
	enc_ctx.avcodec_send_frame(frame);
	FFmpeg::AVPacket pkt;
	while (1)
	{
		try
		{
			enc_ctx.avcodec_receive_packet(pkt);
			FFmpeg::AVPacket pkt1;
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
/// 尝试解封装功能
/// </summary>
void try_remux()
{
	FFmpeg::AVFormatContext iformat_context;
	FFmpeg::AVFormatContext oformat_context;
	iformat_context.open_input("./re.ts");
	oformat_context.alloc_output_context2("output.ts");
	// 查找输入文件的流信息
	iformat_context.find_stream_info();
	// 输入文件的流数量
	int intput_stream_count = iformat_context()->nb_streams;
	// 创建流映射数组
	int* stream_map = new int[intput_stream_count];
	int dst_stream_index = 0;// 流要复制到的目标索引
	// 遍历每一个输入流，对输入流进行过滤，为输出格式创建流，将通过过滤的输入流的信息复制到输出流
	for (int i = 0; i < intput_stream_count; i++)
	{
		FFmpeg::AVStream current_input_stream = iformat_context.get_stream(i);
		// 过滤流。只保留视频流、音频流、字幕流
		if (current_input_stream()->codecpar->codec_type != FFmpeg::AVMediaType::AVMEDIA_TYPE_AUDIO &&
			current_input_stream()->codecpar->codec_type != FFmpeg::AVMediaType::AVMEDIA_TYPE_VIDEO &&
			current_input_stream()->codecpar->codec_type != FFmpeg::AVMediaType::AVMEDIA_TYPE_SUBTITLE)
		{
			// 不是视频、音频、字幕流
			stream_map[i] = -1;
		}
		else
		{
			// 符合过滤条件
			stream_map[i] = dst_stream_index++;
			FFmpeg::AVStream current_output_stream = oformat_context.create_new_stream();
			current_input_stream.copy_parameters_to(current_output_stream);
			current_output_stream()->codecpar->codec_tag = 0;
		}
	}
	// 打印输出格式信息
	oformat_context.dump_format(0, "file", 1);
	cout << "--------------------------" << endl;
	oformat_context.write_header();
	try
	{
		FFmpeg::AVPacket packet;
		while (1)
		{
			iformat_context.read_frame(packet);
			FFmpeg::AVStream input_stream = iformat_context.get_stream(packet()->stream_index);
			// 如果映射表中的目标索引号是负数，表示此流不被复制到输出格式中
			if (stream_map[packet()->stream_index] >= 0)
			{
				// 将此包的流索引号改为目标流索引号，这样这个包就会被写入输出格式的中的目标索引号
				// 的流中
				packet()->stream_index = stream_map[packet()->stream_index];
				FFmpeg::AVStream output_stream = oformat_context.get_stream(packet()->stream_index);
				packet()->pts = av_rescale_q_rnd(packet()->pts, input_stream()->time_base, output_stream()->time_base, ::AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
				packet()->dts = av_rescale_q_rnd(packet()->dts, input_stream()->time_base, output_stream()->time_base, ::AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
				packet()->duration = av_rescale_q(packet()->duration, input_stream()->time_base, output_stream()->time_base);
				packet()->pos = -1;
				oformat_context.interleaved_write_frame(packet);
			}
			packet.unref();
		}
	}
	catch (int err_code)
	{
		cout << "错误码：" << err_code << endl;
		std::cerr << FFmpeg::error_code_to_str(err_code) << endl;
		cout << "写尾部" << endl;
		oformat_context.write_trailer();
	}

	delete[] stream_map;
}

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
	codec_context.avcodec_open2();

	FFmpeg::AVFrame frame;
	frame()->format = codec_context()->pix_fmt;
	frame()->width = codec_context()->width;
	frame()->height = codec_context()->height;
	frame.av_frame_get_buffer(0);
	/* encode 1 second of video */
	for (i = 0; i < 25; i++)
	{
		fflush(stdout);

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

int main(void)
{
	try
	{
		cout << "try_remux" << endl;
		try_remux();
		cout << endl;
		cout << "try_encode" << endl;
		try_encode();
		return 0;
	}
	catch (int err_code)
	{
		std::cerr << FFmpeg::error_code_to_str(err_code) << endl;
	}
	catch (const char* str)
	{
		std::cerr << str << endl;
	}
}
