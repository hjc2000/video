#include<FFmpeg.h>

void try_encode();
void try_remux();
int try_decode_video();
int try_transcode();
int demux_decode_main(const char *src_filename);
using FFmpeg::Exception;

int main(void)
{
	try
	{
		//try_remux();
		//try_decode_video();
		//try_transcode();

		/*
		ffplay -f rawvideo -video_size 1280x720 out_video.yuv
		ffplay -f rawvideo -video_size 352x288 out_video.yuv
		ffplay -f f32le -ac 1 -ar 44100 out_audio.pcm
		// 播放一剪梅
		ffplay -f s16le -ac 1 -ar 44100 out_audio.pcm
		ffplay -f rawvideo -video_size  480x480 out_video.yuv
		*/
		demux_decode_main("mpegts1.mp4");
		//demux_decode_main("input_audio.flac");
		cout << endl;
		cout << "*************************************************" << endl;
		cout << endl;
		//try_encode();



		//demux_decode_main("in.mp4");
		return 0;
	}
	catch (Exception e)
	{
		cout << e.what() << endl;
	}
}
