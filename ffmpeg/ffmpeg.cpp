#include<FFmpeg.h>

void encode_main();
void remux_main();
int demux_decode_main(const char *src_filename);
using FFmpeg::Exception;

int main(void)
{
	try
	{
		#pragma region 转封装
		//remux_main();
		#pragma endregion

		#pragma region 解码 mpegts1.mp4
		/*
		ffplay -f rawvideo -video_size 352x288 out_video.yuv
		*/
		//demux_decode_main("mpegts1.mp4");
		#pragma endregion

		#pragma region 解码等你下课
		/*
		ffplay -f rawvideo -video_size 1280x720 out_video.yuv
		ffplay -f f32le -ac 1 -ar 44100 out_audio.pcm
		*/
		//demux_decode_main("等你下课.mp4");
		#pragma endregion

		#pragma region 解码 8 bit 一剪梅
		/*
		ffplay -f s16le -ac 1 -ar 44100 out_audio.pcm
		ffplay -f rawvideo -video_size  480x480 out_video.yuv
		*/
		//demux_decode_main("input_audio.flac");
		#pragma endregion

		#pragma region 生成裸流并编码
		//encode_main();
		#pragma endregion


		//demux_decode_main("in.mp4");
		return 0;
	}
	catch (Exception e)
	{
		cout << e << endl;
	}
}
