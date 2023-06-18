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
		iformat_context.find_stream_info();
		// 输入文件的流数量
		int intput_stream_num = iformat_context()->nb_streams;
		int *stream_map = new int[intput_stream_num];
		// 遍历每一个输入流，创建 stream_map ，并为输出文件创建流
		for (int i = 0; i < intput_stream_num; i++)
		{
		}
		delete[] stream_map;
		return 0;
	}
	catch (int err_code)
	{
		std::cerr << FFmpeg::error_code_to_str(err_code) << endl;
	}
	catch (const char *str)
	{
		std::cerr << str << endl;
	}
}
