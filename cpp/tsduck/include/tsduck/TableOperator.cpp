#include"tsduck/TableOperator.h"

std::vector<ts::TSPacket> video::TableOperator::ToTsPacket(
	ts::DuckContext &duck,
	ts::BinaryTable const &table,
	uint16_t pid
)
{
	std::vector<ts::TSPacket> pat_packets;
	ts::OneShotPacketizer pat_packetizer{ duck, pid };
	pat_packetizer.addTable(table);
	pat_packetizer.getPackets(pat_packets);
	return pat_packets;
}

std::vector<ts::TSPacket> video::TableOperator::ToTsPacket(ts::DuckContext &duck, ts::PAT const &table)
{
	ts::BinaryTable out_table;
	table.serialize(duck, out_table);
	return ToTsPacket(duck, out_table, 0);
}

std::vector<ts::TSPacket> video::TableOperator::ToTsPacket(ts::DuckContext &duck, ts::PMT const &table, uint16_t pid)
{
	ts::BinaryTable out_table;
	table.serialize(duck, out_table);
	return ToTsPacket(duck, out_table, pid);
}

std::vector<ts::TSPacket> video::TableOperator::ToTsPacket(ts::DuckContext &duck, ts::SDT const &table)
{
	ts::BinaryTable out_table;
	table.serialize(duck, out_table);
	return ToTsPacket(duck, out_table, 0x11);
}

void video::TableOperator::RemoveRedundantServicesFromSdt(ts::SDT &sdt, ts::PAT const &pat)
{
	std::vector<uint16_t> services_to_remove;

	/* 遍历 sdt.services，找出在 pat 中没有指示的，将 service_id 添加到待移除列表。
	* 不能在迭代的同时修改集合。
	*/
	for (auto &service_pair : sdt.services)
	{
		if (pat.pmts.find(service_pair.first) == pat.pmts.end())
		{
			services_to_remove.push_back(service_pair.first);
		}
	}

	// 从 SDT 中删除服务。
	for (uint16_t service_id : services_to_remove)
	{
		sdt.services.erase(service_id);
	}
}

ts::PIDSet &operator<<(ts::PIDSet &set, ts::PMT const &pmt)
{
	set[pmt.pcr_pid] = 1;
	for (auto &it : pmt.streams)
	{
		set[it.first] = 1;
	}

	return set;
}

ts::PAT &operator<<(ts::PAT &pat, ts::PAT const &another_pat)
{
	for (auto &it : another_pat.pmts)
	{
		pat.pmts[it.first] = it.second;
	}

	return pat;
}

ts::SDT &operator<<(ts::SDT &sdt, ts::SDT const &another_sdt)
{
	for (auto &it : another_sdt.services)
	{
		sdt.services[it.first] = it.second;
	}

	return sdt;
}
