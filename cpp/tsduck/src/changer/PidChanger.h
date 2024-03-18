#pragma once
#include<Exception.h>
#include<ITSPacketConsumer.h>
#include<PipeTsPacketSource.h>
#include<TableOperator.h>
#include<TableVersionChangeHandler.h>
#include<TsDuckToString.h>
#include<map>
#include<tsBinaryTable.h>
#include<tsDuckContext.h>
#include<tsSectionDemux.h>

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
		public ITsPacketConsumer,
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
		PidChanger(std::map<uint16_t, uint16_t> pid_map);

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
		using ITsPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override;

		/// <summary>
		///		如果表解析已经完成，这里设置后不会生效，必须送进来新版本的 PAT 才能生效。
		/// 
		///		* 可以在每次送入新的 PMT 之前设置 pid_map，映射规则会对新送进来的
		///		  PMT 中指示的流 PID 和 PCR PID 生效。
		/// </summary>
		/// <param name="pid_map"></param>
		void SetPidMap(std::map<uint16_t, uint16_t> const &pid_map);
	};
}