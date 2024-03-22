#include"ProgramDecoder.h"

using namespace video;

void video::ProgramDecoder::SendPacket(AVPacketWrapper *packet)
{
	if (!packet)
	{
		// packet 为空指针，冲洗模式。
		for (std::pair<int const, shared_ptr<DecoderPipe>> &pair : _decode_pipe_map)
		{
			pair.second->SendPacket(nullptr);
		}

		return;
	}

	// packet 不为空指针。

	/* 根据包的流索引，在解码管道 map 中找到对应的解码管道，如果找不到，则直接返回，不送入包。*/
	auto it = _decode_pipe_map.find(packet->stream_index());
	if (it == _decode_pipe_map.end())
	{
		cout << CODE_POS_STR << "根据送入的包的流索引，在 map 中找不到解码管道。" << endl;
		return;
	}

	shared_ptr<DecoderPipe> decode_pipe = it->second;
	if (!decode_pipe)
	{
		cout << CODE_POS_STR << "解码管道 map 中有该键，但却没有值。" << endl;
		throw jc::Exception();
	}

	// 成功拿到此包对应的流的解码管道了
	decode_pipe->SendPacket(packet);
}

void video::ProgramDecoder::ReplaceDecodePipe(List<AVStreamInfoCollection> stream_list)
{
	_stream_list = stream_list;

	// 遍历每一个流
	for (AVStreamInfoCollection &stream : _stream_list)
	{
		// 根据流索引，在 _decode_pipe_map 中查找属于该流的解码管道。
		auto it = _decode_pipe_map.find(stream._index);
		if (it == _decode_pipe_map.end())
		{
			// 没找到，继续对 _stream_list 的下一个流进行操作。
			continue;
		}

		// 找到该流的解码管道了
		shared_ptr<DecoderPipe> old_decode_pipe = it->second;

		// 创建新的解码管道，让它继承旧的解码管道的消费者
		shared_ptr<DecoderPipe> new_decode_pipe{ new DecoderPipe{stream} };
		new_decode_pipe->AddConsumerFromAnotherPipe(*old_decode_pipe);

		// 冲洗旧的解码管道，但是不冲洗它们的消费者。
		old_decode_pipe->FlushDecoderButNotFlushConsumers();

		// 将新的解码管道放入 map 中，替换掉旧的解码管道。
		it->second = new_decode_pipe;
	}
}

List<AVStreamInfoCollection> &video::ProgramDecoder::StreamList()
{
	return _stream_list;
}

shared_ptr<IPipeFrameSource> video::ProgramDecoder::GetPipeFrameSourceByStreamIndex(int index)
{
	auto it = _decode_pipe_map.find(index);
	if (it == _decode_pipe_map.end())
	{
		return nullptr;
	}

	return it->second;
}
