#pragma once
#include<AVStreamInfoCollection.h>
#include<PipeFrameSource.h>

namespace video
{
	/// <summary>
	///		多路流的 IPipeFrameSource 集合。每一路流都有一个 IPipeFrameSource
	///		对象，可以从中读取出 AVFrame。
	/// </summary>
	class IMultiStreamPipeFrameSource
	{
	public:
		virtual ~IMultiStreamPipeFrameSource() {}

	public:
		virtual List<AVStreamInfoCollection> &StreamList() = 0;
		virtual shared_ptr<IPipeFrameSource> GetPipeFrameSourceByStreamIndex(int stream_index) = 0;
	};
}
