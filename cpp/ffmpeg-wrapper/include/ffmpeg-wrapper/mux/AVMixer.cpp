#include"AVMixer.h"

using namespace std;
using namespace video;

video::AVMixer::AVMixer(
	shared_ptr<InputFormat> input_video_format,
	shared_ptr<InputFormat> input_audio_format,
	shared_ptr<OutputFormat> out_format
)
{
	_input_video_format = input_video_format;
	_input_audio_format = input_audio_format;
	_out_format = out_format;
}
