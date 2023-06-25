#include<FFmpeg.h>

void try_encode();
void try_remux();
int try_decode_video();
int try_transcode();
int demux_decode();

int main(void)
{
	try
	{
		//try_remux();
		//try_encode();
		//try_decode_video();
		//try_transcode();

		// ffplay -f rawvideo -video_size 352*288 out_video.yuv
		demux_decode();
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
