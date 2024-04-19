#pragma once
#include<ffmpeg-wrapper/input-format/InputFormat.h>
#include<ffmpeg-wrapper/output-format/OutputFormat.h>
#include<ffmpeg-wrapper/pipe/interface/IPump.h>

namespace video
{
	/// <summary>
	///		接受一个音频输入格式，一个视频输入格式，将两个输入格式解封装，将它们的包混合在一起，
	///		写入输出格式。
	/// 
	///		视频包的流索引会调整为 0，音频包的流索引会调整为 1.
	/// </summary>
	class AVMixer :public IPump
	{
		shared_ptr<InputFormat> _input_video_format;
		shared_ptr<InputFormat> _input_audio_format;
		shared_ptr<OutputFormat> _out_format;

		/* 音视频的起始时间戳可能不对齐，需要都调整到 0。在收到第一个包的时候就要将时间戳记录下来，
		* 之后的每个包都要减去记录的时间戳。
		*
		* 包的时间戳有 2 个，即 pts 和 dts。要选择这两个中的最小值作为偏移量，如果选择最大值作为
		* 偏移量，会导致其中一个时间戳为负数。
		*/
		int64_t _video_time_stamp_offset = 0;
		int64_t _audio_time_stamp_offset = 0;
		bool _is_first_video_packet = true;
		bool _is_first_audio_packet = true;

		void CreateNewVideoStream();

	public:
		AVMixer(
			shared_ptr<InputFormat> input_video_format,
			shared_ptr<InputFormat> input_audio_format,
			shared_ptr<OutputFormat> out_format
		);

		void Pump(shared_ptr<CancellationToken> cancel_pump) override;
	};
}