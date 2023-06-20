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
		oformat_context.alloc_output_context2("./out.mp4");
		// 查找输入文件的流信息
		iformat_context.find_stream_info();
		// 输入文件的流数量
		int intput_stream_count = iformat_context()->nb_streams;
		// 创建流映射数组
		int* stream_map = new int[intput_stream_count];
		int dst_stream_index = 0;// 流要复制到的目标索引
		// 遍历每一个输入流
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
				oformat_context.dump_format(0, "file", 1);
				cout << "--------------------------" << endl;
				if (!(oformat_context()->oformat->flags & AVFMT_NOFILE))
				{
					cout << "没有打开文件" << endl;
					//ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
					//if (ret < 0)
					//{
					//	fprintf(stderr, "Could not open output file '%s'", out_filename);
					//	goto end;
					//}
				}

				//out_stream = avformat_new_stream(ofmt_ctx, NULL);
				//if (!out_stream)
				//{
				//	fprintf(stderr, "Failed allocating output stream\n");
				//	ret = AVERROR_UNKNOWN;
				//	goto end;
				//}

				//ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
				//if (ret < 0)
				//{
				//	fprintf(stderr, "Failed to copy codec parameters\n");
				//	goto end;
				//}
				//out_stream->codecpar->codec_tag = 0;
			}
		}
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
