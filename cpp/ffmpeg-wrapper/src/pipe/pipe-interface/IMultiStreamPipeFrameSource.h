#pragma once
#include<AVStreamInfoCollection.h>
#include<PipeFrameSource.h>

namespace video
{
	class IMultiStreamPipeFrameSource
	{
	public:
		virtual ~IMultiStreamPipeFrameSource() {}

	public:
		/// <summary>
		///		流列表。每个流都对应一个 IPipeFrameSource 对象，可以从中读出对应的流的帧。通过
		///		GetPipeFrameSourceByStreamIndex 方法来获取 IPipeFrameSource 对象。
		/// </summary>
		/// <returns></returns>
		virtual List<AVStreamInfoCollection> &StreamList() = 0;

		/// <summary>
		///		应该配合 StreamList 属性来使用。遍历 StreamList 中的每一个流，
		///		然后取得流索引后再来调用本函数获取 IPipeFrameSource，这样就可以避免返回空指针。
		/// </summary>
		/// <param name="stream_index"></param>
		/// <returns>如果指定的流索引有相应的 IPipeFrameSource，则返回 IPipeFrameSource，否则返回空指针。</returns>
		virtual shared_ptr<IPipeFrameSource> GetPipeFrameSourceByStreamIndex(int stream_index) = 0;
	};
}