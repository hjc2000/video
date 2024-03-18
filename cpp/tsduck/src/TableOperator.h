#pragma once
#include<chrono>
#include<tsOneShotPacketizer.h>
#include<tsPAT.h>
#include<tsPMT.h>
#include<tsSDT.h>
#include<tsTSPacket.h>
#include<vector>

namespace video
{
	class TableOperator
	{
	private:
		TableOperator() = delete;

	public:
		/// <summary>
		///		将表格转化为一系列 ts 包，放到向量中并返回。可以遍历向量，将 ts 包写入 ts。
		/// </summary>
		/// <param name="duck"></param>
		/// <param name="table"></param>
		/// <returns></returns>
		static std::vector<ts::TSPacket> ToTsPacket(
			ts::DuckContext &duck,
			ts::BinaryTable const &table,
			uint16_t pid
		);

		/// <summary>
		///		将表格转化为一系列 ts 包，放到向量中并返回。可以遍历向量，将 ts 包写入 ts。
		/// </summary>
		/// <param name="duck"></param>
		/// <param name="table"></param>
		/// <returns></returns>
		static std::vector<ts::TSPacket> ToTsPacket(ts::DuckContext &duck, ts::PAT const &table);

		/// <summary>
		///		将表格转化为一系列 ts 包，放到向量中并返回。可以遍历向量，将 ts 包写入 ts。
		/// </summary>
		/// <param name="duck"></param>
		/// <param name="table"></param>
		/// <returns></returns>
		static std::vector<ts::TSPacket> ToTsPacket(ts::DuckContext &duck, ts::PMT const &table, uint16_t pid);

		/// <summary>
		///		将表格转化为一系列 ts 包，放到向量中并返回。可以遍历向量，将 ts 包写入 ts。
		/// </summary>
		/// <param name="duck"></param>
		/// <param name="table"></param>
		/// <returns></returns>
		static std::vector<ts::TSPacket> ToTsPacket(ts::DuckContext &duck, ts::SDT const &table);

		/// <summary>
		///		将 pat 中没有指示的多余的服务从 sdt 中移除。
		/// </summary>
		/// <param name="sdt">此 SDT 会被修改。</param>
		/// <param name="pat"></param>
		static void RemoveRedundantServicesFromSdt(ts::SDT &sdt, ts::PAT const &pat);
	};
}

/// <summary>
///		将 PMT 的 PCR PID 和各个流的 PID 添加到 PIDSet 中。
/// </summary>
/// <param name="set"></param>
/// <param name="pmt"></param>
ts::PIDSet &operator<<(ts::PIDSet &set, ts::PMT const &pmt);

/// <summary>
///		合并两个 PAT。
///		* 遍历 another_pat 的 pmts 复制到 pat 的 pmts 中。会覆盖。
/// </summary>
/// <param name="pat"></param>
/// <param name="another_pat"></param>
/// <returns></returns>
ts::PAT &operator<<(ts::PAT &pat, ts::PAT const &another_pat);

/// <summary>
///		将 another_sdt 的 services 合并到 sdt 中。会改变 sdt。
/// </summary>
/// <param name="sdt"></param>
/// <param name="another_sdt"></param>
/// <returns></returns>
ts::SDT &operator<<(ts::SDT &sdt, ts::SDT const &another_sdt);
