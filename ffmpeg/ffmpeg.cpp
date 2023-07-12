#include<FFmpeg.h>
// 如果不定义这个宏，且 main 函数中没有接收命令行，会报错
#define SDL_MAIN_HANDLED
#include <SDL.h>

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
		demux_decode_main("input_audio.flac");
		#pragma endregion

		#pragma region 解码来自网络的视频
		/* 可以使用下面这条命令查看 PCM 的格式字符串
		* ffmpeg -formats | findstr PCM
		* 格式字符串是使用 ffplay 播放 PCM 文件时要用的

		ffplay -f rawvideo -video_size  1920*1080 out_video.yuv
		ffplay -f f32le -ac 1 -ar 44100 out_audio.pcm
		*/
		//demux_decode_main("http://localhost:8848/qq.mp4");
		#pragma endregion

		#pragma region 生成裸流并编码
		//encode_main();
		#pragma endregion


		//demux_decode_main("in.mp4");

		//const int screen_width = 1280;
		//const int screen_height = 720;
		//SDL_Init(SDL_INIT_VIDEO);
		//SDL_Window *window = SDL_CreateWindow("YourGame",
		//	SDL_WINDOWPOS_UNDEFINED,
		//	SDL_WINDOWPOS_UNDEFINED,
		//	screen_width, screen_height,
		//	SDL_WINDOW_SHOWN);
		//SDL_Surface *surface = SDL_GetWindowSurface(window);
		//SDL_Surface *blackground_surface = SDL_LoadBMP("Hello.bmp");
		//SDL_BlitSurface(blackground_surface, NULL, surface, NULL);
		//SDL_UpdateWindowSurface(window);
		//SDL_Delay(3000);
		//SDL_DestroyWindow(window);
		//SDL_Quit();

		return 0;
	}
	catch (Exception e)
	{
		cout << e << endl;
	}
}
