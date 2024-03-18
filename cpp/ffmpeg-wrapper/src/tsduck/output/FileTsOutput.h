#pragma once
#include<ITsPacketConsumer.h>
#include<string>

namespace video
{
	/// <summary>
	///		送入 ts 包，会输出到文件。
	/// </summary>
	class FileTsOutput :public ITsPacketConsumer
	{
	public:
		/// <summary>
		///		每次构造都会创建新文件，如果已经存在，会覆盖原来的。
		/// </summary>
		/// <param name="out_file_url"></param>
		FileTsOutput(std::string out_file_url)
		{
			_out_fs = FileStream::CreateNewAnyway(out_file_url.c_str());
		}

	private:
		shared_ptr<FileStream> _out_fs;

	public:
		/// <summary>
		///		送入包，会被写入文件。
		/// </summary>
		void SendPacket(ts::TSPacket *packet) override
		{
			_out_fs->Write(packet->b, 0, 188);
		}
	};
}