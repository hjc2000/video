#pragma once
#include<ffmpeg-wrapper/input-format/InputFormat.h>
#include<ffmpeg-wrapper/output-format/OutputFormat.h>

namespace video
{
	/// <summary>
	///		接受一个音频输入格式，一个视频输入格式，将两个输入格式解封装，将它们的包混合在一起，
	///		写入输出格式。
	/// 
	///		视频包的流索引会调整为 0，音频包的流索引会调整为 1.
	/// </summary>
	class AVMixer
	{
	public:
		AVMixer(
			shared_ptr<InputFormat> input_video_format,
			shared_ptr<InputFormat> input_audio_format,
			shared_ptr<OutputFormat> out_format
		);
	};
}