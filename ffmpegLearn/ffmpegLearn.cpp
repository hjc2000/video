#include <FFmpeg.h>
#include <iostream>
using namespace FFmpeg;

int main(void)
{
	try
	{
		FFmpeg::AVFormatContext inputFormat = FFmpeg::AVFormatContext("./in1.mp4", FFmpeg::FormatMode::Open);
		FFmpeg::AVFormatContext outputFormat = FFmpeg::AVFormatContext("./out.ts", FFmpeg::FormatMode::Create);
		outputFormat.CreateNewStream(2);
		std::vector<std::shared_ptr<FFmpeg::AVStream>> streams = inputFormat.get_StreamList();
		streams[0]->set_DstStreamIndex(0);
		streams[1]->set_DstStreamIndex(1);
		inputFormat.SeekFrame(100);
		inputFormat.CopyStreamTo(outputFormat, 120);
	}
	catch (int errCode)
	{
		char buff[200];
		av_strerror(errCode, buff, sizeof(buff));
		std::cerr << buff << endl;
	}

	return 0;
}
