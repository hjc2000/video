#pragma once
#include<tsBinaryTable.h>
#include<tsCerrReport.h>
#include<tsDuckContext.h>
#include<tsPAT.h>
#include<tsPMT.h>
#include<tsSectionDemux.h>

using std::shared_ptr;

namespace video
{
	/// <summary>
	///		继承此类，然后派生类将 ts 包送给 _demux，就会解析表格，然后触发相应的回调。
	///		回调就是本类的虚函数，派生类可以重写。
	/// </summary>
	class TableHandler :
		public ts::TableHandlerInterface
	{
	public:
		TableHandler();
		virtual ~TableHandler() {}

	private:
		void handleTable(ts::SectionDemux &demux, ts::BinaryTable const &table) final override;
		ts::PIDSet _default_listened_pid_set;

	protected:
		shared_ptr<ts::DuckContext> _duck;
		shared_ptr<ts::SectionDemux> _demux;

		virtual void HandlePAT(ts::BinaryTable const &table) {}
		virtual void HandlePMT(ts::BinaryTable const &table) {}
		virtual void HandleSDT(ts::BinaryTable const &table) {}

		/// <summary>
		///		重置监听的 PID。重置后将会仅监听几个默认的 PID。
		/// </summary>
		void ResetListenedPids();

		/// <summary>
		///		监听 pat 中指示的 PMT PID。
		/// </summary>
		/// <param name="pat"></param>
		void ListenOnPmtPids(ts::PAT const &pat);
	};
}
