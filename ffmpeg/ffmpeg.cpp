#include <AVDictionary.h>
#include <AVFormatContext.h>
#include <AVError.h>
#include <AVUtil.h>
#include <AVStream.h>
#include <AVPacket.h>
extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <libavutil/log.h>
#include <libavutil/error.h>
#include <libavutil/avutil.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
}
#include <iostream>
#include <fstream>
#include <string>
using std::cout;
using std::endl;
using std::fstream;
using std::ios_base;
using std::string;

int main(void)
{
	try
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
		if (!(oformat_context()->oformat->flags & AVFMT_NOFILE))
		{
			int result = avio_open(&oformat_context()->pb, "output.ts", AVIO_FLAG_WRITE);
			if (result < 0)
				throw result;
		}
		oformat_context.write_header();
		try
		{
			while (1)
			{
				FFmpeg::AVPacket packet = iformat_context.read_frame();
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
			}
		}
		catch (int err_code)
		{
			cout << "错误码：" << err_code << endl;
			std::cerr << FFmpeg::error_code_to_str(err_code) << endl;
			cout << "写尾部" << endl;
			oformat_context.write_trailer();
		}

		if (!(oformat_context()->oformat->flags & AVFMT_NOFILE))
			avio_closep(&oformat_context()->pb);
		delete[] stream_map;
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
