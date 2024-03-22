#pragma once
#include<AVProgramWrapper.h>
#include<AVStreamInfoCollection.h>
#include<AVStreamWrapper.h>
#include<DecoderPipe.h>
#include<IMultiStreamPipeFrameSource.h>
#include<IPacketConsumer.h>
#include<List.h>
#include<map>

namespace video
{
	/// <summary>
	///		多个流组成一个节目。本类对象专门针对 List<AVStreamInfoCollection>，会为每一路流创建解码
	///		管道，并且使用 map 来映射流索引和解码管道。
	/// </summary>
	class ProgramDecoder :
		public IPacketConsumer,
		public IMultiStreamPipeFrameSource
	{
	public:
		/// <summary>
		///		本对象会为 stream_list 中的每一个流创建解码管道。
		///		* stream_list 里的流不一定要是一个 mpegts 节目里的流，可以是任意的组合。
		/// </summary>
		/// <param name="stream_list"></param>
		ProgramDecoder(List<AVStreamInfoCollection> stream_list)
		{
			_stream_list = stream_list;

			// 为列表中的每一路流创建解码管道并放入 map
			for (AVStreamInfoCollection &stream : _stream_list)
			{
				shared_ptr<DecoderPipe> decode_pipe{ new DecoderPipe{stream} };
				_decode_pipe_map[stream._index] = decode_pipe;
			}
		}

	private:
		List<AVStreamInfoCollection> _stream_list;

		/// <summary>
		///		解码管道的字典。
		///		* 键为 AVStreamWrapper 对象的 Index 属性。
		/// </summary>
		std::map<int, shared_ptr<DecoderPipe>> _decode_pipe_map;

	public:
		void SendPacket(AVPacketWrapper *packet) override;

		/// <summary>
		///		流的参数有变，需要更换解码管道。
		/// </summary>
		/// <param name="stream_list"></param>
		void ReplaceDecodePipe(List<AVStreamInfoCollection> stream_list);

		#pragma region 通过 IMultiStreamPipeFrameSource 继承
		List<AVStreamInfoCollection> &StreamList() override;
		shared_ptr<IPipeFrameSource> GetPipeFrameSourceByStreamIndex(int index) override;
		#pragma endregion

	};
}