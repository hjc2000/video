#pragma once
#include<TableHandler.h>
#include<TableOperator.h>
#include<functional>

namespace video
{
	/// <summary>
	///		继承此类，然后派生类将 ts 包送给 _demux，就会解析表格，然后触发相应的回调。
	///		回调就是本类的虚函数，派生类可以重写。
	/// </summary>
	class TableVersionChangeHandler :
		public TableHandler
	{
	private:
		int _pat_version = -1;
		int _sdt_version = -1;

		/// <summary>
		///		key=pmt_pid, value=pmt_version
		/// </summary>
		std::map<uint16_t, uint8_t> _pmt_versions;
		ts::PAT _current_pat;

	private:
		void HandlePAT(ts::BinaryTable const &table) final override;
		void HandlePMT(ts::BinaryTable const &table) final override;
		void HandleSDT(ts::BinaryTable const &table) final override;

	protected:
		/// <summary>
		///		收到 PMT 后，会将 PMT 中指示的流的 PID 和 PRC PID 放到这里。
		/// </summary>
		ts::PIDSet _streams_pid_set;

		virtual void HandlePatVersionChange(ts::PAT &pat) {}
		virtual void HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid) {}
		virtual void HandleSdtVersionChange(ts::BinaryTable const &table) {}

	public:
		/// <summary>
		///		版本发生变化后，在调用 HandlePatVersionChange 之前会触发此回调。
		///		回调时会传入参数：
		///			* current_pat：当前版本的 PAT。此时还未处理新的 PAT。
		///			* new_pat：新版本的 PAT。此时还没被处理。
		/// </summary>
		std::function<void(ts::PAT const &current_pat, ts::PAT &new_pat)> _on_before_handling_new_version_pat;
	};
}