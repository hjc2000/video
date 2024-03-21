#include"AutoPidChanger.h"

using namespace std;
using namespace video;

#pragma region 内部类型
/// <summary>
///		能够改变 PMT 和各个流的 PID。如果被更改 PID 的流携带了 PCR，则会自动更改 PMT 中的 pcr_pid 字段。
///		一般 PCR 在视频流中。
/// 
///		* 输出端需要串联 PatPmtRepeater，本类不会重复发送 PAT 和 PMT，除非这两个表版本更新。所有 *Changer 类都一样，
///		  只专注于修改，不专注重复 PAT 和 PMT。
/// </summary>
class video::AutoPidChanger::PidChanger :
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
	void ChangeStreamPidAndPcrPid(ts::PMT &pmt)
	{
		// 更改 PCR PID。
		auto it = _pid_map.find(pmt.pcr_pid);
		if (it != _pid_map.end())
		{
			// 如果映射表中找得到 PCR PID，将 PCR PID 改掉。
			pmt.pcr_pid = it->second;
		}

		// 更改各个流的 PID。
		ts::PMT backup_pmt{ pmt };
		for (auto &map_pair : _pid_map)
		{
			auto it = pmt.streams.find(map_pair.first);
			if (it != pmt.streams.end())
			{
				// 如果该 PMT 里有流的 PID 在映射表里找得到，删除该流。
				pmt.streams.erase(it);
			}
		}

		for (auto &map_pair : _pid_map)
		{
			auto it = backup_pmt.streams.find(map_pair.first);
			if (it != backup_pmt.streams.end())
			{
				/* 如果先前备份的 PMT 中有流的 PID 在映射表中找得到，
				将这个流用映射后的 PID 放入原来的 PMT 中。*/
				pmt.streams[map_pair.second] = it->second;
			}
		}
	}

	void HandlePatVersionChange(ts::PAT &pat) override
	{
		// 更改 PMT PID。
		for (auto &pmt : pat.pmts)
		{
			// 看映射表里面有没有该 PMT
			auto it = _pid_map.find(pmt.second);
			if (it != _pid_map.end())
			{
				// 更改 PAT 中此 PMT 的 PID。
				pmt.second = it->second;
			}
		}

		SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, pat));
	}

	void HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid) override
	{
		ChangeStreamPidAndPcrPid(pmt);

		// 更改 PMT PID。
		uint16_t out_pmt_pid = source_pid;
		auto it = _pid_map.find(source_pid);
		if (it != _pid_map.end())
		{
			out_pmt_pid = it->second;
		}

		SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, pmt, out_pmt_pid));
	}

public:
	using ITSPacketConsumer::SendPacket;
	void SendPacket(ts::TSPacket *packet) override
	{
		_demux->feedPacket(*packet);
		if (_streams_pid_set[packet->getPID()])
		{
			uint16_t src_pid = packet->getPID();
			auto it = _pid_map.find(src_pid);
			if (it != _pid_map.end())
			{
				packet->setPID(it->second);
			}

			SendPacketToEachConsumer(packet);

			// 因为传进来的是指针，刚才把 PID 改了，现在要恢复。
			packet->setPID(src_pid);
		}
		else if (packet->getPID() == 0x11)
		{
			SendPacketToEachConsumer(packet);
		}
	}

	/// <summary>
	///		调用者可能需要边解析 ts 边设置映射规则。例如，解析完 PAT 后可以先设置
	///		PMT PID 的映射规则。等到解析到 PMT 时才有办法设置 PCR PID 和各个流的映射规则。
	///		所以这个函数提供了一个更改映射表的途径。
	/// </summary>
	/// <param name="pid_map"></param>
	void SetPidMap(std::map<uint16_t, uint16_t> const &pid_map)
	{
		auto it = pid_map.find(0);
		if (it != pid_map.end())
		{
			throw jc::Exception("不允许更改 PAT 的 PID。");
		}

		it = pid_map.find(0x11);
		if (it != pid_map.end())
		{
			throw jc::Exception("不允许更改 SDT 的 PID。");
		}

		_pid_map = pid_map;
	}
};
#pragma endregion






video::AutoPidChanger::AutoPidChanger(
	shared_ptr<PidProvider> pid_provider,
	std::map<uint16_t, uint16_t> const &preset_pid_map
)
{
	_pid_provider = pid_provider;
	_preset_pid_map = preset_pid_map;
	for (auto &it : _preset_pid_map)
	{
		it.second = pid_provider->GetPid(it.second);
	}
}

void video::AutoPidChanger::HandlePatVersionChange(ts::PAT &pat)
{
	/* 遍历 _final_pid_map，找出 _preset_pid_map 中没有的，归还映射到的目标 PID。
	* 注意，映射到的目标 PID 是迭代器的 second 字段。
	*/
	for (auto &it : _final_pid_map)
	{
		if (_preset_pid_map.find(it.first) == _preset_pid_map.end())
		{
			_pid_provider->ReturnPid(it.second);
		}
	}

	_final_pid_map = _preset_pid_map;
	for (auto &pmt : pat.pmts)
	{
		if (_final_pid_map.find(pmt.second) == _final_pid_map.end())
		{
			_final_pid_map[pmt.second] = _pid_provider->GetPid(pmt.second);
		}
	}

	_pid_changer = shared_ptr<PidChanger>{ new PidChanger{_final_pid_map} };
	_pid_changer->AddTsPacketConsumerFromAnother(*this);
	_pid_changer->SendPacket(TableOperator::ToTsPacket(*_duck, pat));
}

void video::AutoPidChanger::HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid)
{
	for (auto &stream : pmt.streams)
	{
		if (_final_pid_map.find(stream.first) == _final_pid_map.end())
		{
			_final_pid_map[stream.first] = _pid_provider->GetPid(stream.first);
		}
	}

	_pid_changer->SetPidMap(_final_pid_map);
	_pid_changer->SendPacket(TableOperator::ToTsPacket(*_duck, pmt, source_pid));
}

#pragma region PipeTsPacketSource
void video::AutoPidChanger::AddTsPacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer)
{
	PipeTsPacketSource::AddTsPacketConsumer(packet_comsumer);
	if (_pid_changer)
	{
		_pid_changer->AddTsPacketConsumer(packet_comsumer);
	}
}

bool video::AutoPidChanger::RemovePacketComsumer(shared_ptr<ITSPacketConsumer> packet_comsumer)
{
	bool ret = false;
	ret |= PipeTsPacketSource::RemovePacketComsumer(packet_comsumer);
	if (_pid_changer)
	{
		ret |= _pid_changer->RemovePacketComsumer(packet_comsumer);
	}

	return ret;
}

void video::AutoPidChanger::ClearConsumers()
{
	PipeTsPacketSource::ClearConsumers();
	if (_pid_changer)
	{
		_pid_changer->ClearConsumers();
	}
}
#pragma endregion

void video::AutoPidChanger::SendPacket(ts::TSPacket *packet)
{
	_demux->feedPacket(*packet);
	if (_pid_changer)
	{
		if (_streams_pid_set[packet->getPID()])
		{
			_pid_changer->SendPacket(packet);
		}
		else if (packet->getPID() == 0x11)
		{
			_pid_changer->SendPacket(packet);
		}
	}
}
