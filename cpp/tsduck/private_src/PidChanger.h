#pragma once
#include<tsduck/interface/ITSPacketConsumer.h>
#include<tsduck/PipeTsPacketSource.h>
#include<tsduck/handler/TableVersionChangeHandler.h>

namespace video
{
	/// <summary>
	///		能够改变 PMT 和各个流的 PID。如果被更改 PID 的流携带了 PCR，则会自动更改 PMT 中的 pcr_pid 字段。
	///		一般 PCR 在视频流中。
	/// 
	///		* 输出端需要串联 PatPmtRepeater，本类不会重复发送 PAT 和 PMT，除非这两个表版本更新。所有 *Changer 类都一样，
	///		  只专注于修改，不专注重复 PAT 和 PMT。
	/// </summary>
	class PidChanger :
		public ITSPacketConsumer,
		public PipeTsPacketSource,
		public TableVersionChangeHandler
	{
	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="pid_map">
		///		键为原始 PID，值为要被修改成的 PID。
		/// </param>
		PidChanger(std::map<uint16_t, uint16_t> pid_map)
		{
			SetPidMap(pid_map);
		}

	private:
		std::map<uint16_t, uint16_t> _pid_map;

		/// <summary>
		///		更改 PMT 中各个流的 PID。
		/// </summary>
		/// <param name="pmt"></param>
		void ChangeStreamPidAndPcrPid(ts::PMT &pmt);

		void HandlePatVersionChange(ts::PAT &pat) override;
		void HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid) override;

	public:
		using ITSPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override;

		/// <summary>
		///		调用者可能需要边解析 ts 边设置映射规则。例如，解析完 PAT 后可以先设置
		///		PMT PID 的映射规则。等到解析到 PMT 时才有办法设置 PCR PID 和各个流的映射规则。
		///		所以这个函数提供了一个更改映射表的途径。
		/// </summary>
		/// <param name="pid_map"></param>
		void SetPidMap(std::map<uint16_t, uint16_t> const &pid_map);
	};
}