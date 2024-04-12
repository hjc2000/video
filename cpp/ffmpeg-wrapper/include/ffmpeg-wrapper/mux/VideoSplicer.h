#pragma once
#include<ffmpeg-wrapper/demux/JoinedInputFormatDemuxDecoder.h>
#include<ffmpeg-wrapper/input-format/InputFormat.h>
#include<ffmpeg-wrapper/mux/SptsEncodeMux.h>
#include<ffmpeg-wrapper/output-format/StreamOutputFormat.h>
#include<jccpp/container/List.h>
#include<jccpp/stream/FileStream.h>

namespace video
{
	/// <summary>
	///		视频拼接器。将多个视频拼接成 ts。
	/// </summary>
	class VideoSplicer
	{
	public:

	};
}