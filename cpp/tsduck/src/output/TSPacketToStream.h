#pragma once
#include<ITsPacketConsumer.h>
#include<Stream.h>

namespace video
{
	/// <summary>
	///		送入 ts 包，会输出到文件。
	/// </summary>
	class TSPacketToStream :public ITsPacketConsumer
	{
	public:
		TSPacketToStream(shared_ptr<Stream> out_stream)
		{
			if (out_stream == nullptr)
			{
				throw jc::ArgumentNullException("不允许传入空指针");
			}

			_out_stream = out_stream;
		}

	private:
		shared_ptr<Stream> _out_stream;

	public:
		using ITsPacketConsumer::SendPacket;
		/// <summary>
		///		送入包，会被写入文件。
		/// </summary>
		void SendPacket(ts::TSPacket *packet) override
		{
			_out_stream->Write(packet->b, 0, 188);
		}
	};
}