#pragma once
#include<AutoPidChanger.h>
#include<AutoServiceIdChanger.h>
#include<ProgramMux.h>

namespace video
{
	/// <summary>
	///		节目复用器。能够输入多个节目，自动处理 service_id 和 pid 的冲突。冲突的会被映射到一个
	///		不同的值上。也可以自定义一个预设的映射表，会尽量遵守该预设映射表，除非实在有冲突。
	///		
	///		实际上也只有不同 ts 之间的节目才会冲突，所以本类也可以算作 ts 混合器。
	/// </summary>
	class AutoChangeIdProgramMux :public IPipeTsPacketSource
	{
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="preset_pid_map">
		///		预设的 PID 映射表。如果有期望的自定义规则可以设置此参数。会尽量使用此映射表里定义的规则，
		///		如果有冲突，就会不遵守它里面定义的规则。
		/// </param>
		/// <param name="preset_service_id_map">
		///		预设的 service_id 映射表。如果有期望的自定义规则可以设置此参数。会尽量使用此映射表里定义的规则，
		///		如果有冲突，就会不遵守它里面定义的规则。
		/// </param>
		AutoChangeIdProgramMux(
			std::map<uint16_t, uint16_t> const &preset_pid_map = std::map<uint16_t, uint16_t>{},
			std::map<uint16_t, uint16_t> const &preset_service_id_map = std::map<uint16_t, uint16_t>{}
		);

	private:
		shared_ptr<ProgramMux> _program_mux{ new ProgramMux{} };
		shared_ptr<PidProvider> _pid_provider{ new PidProvider{} };
		shared_ptr<ServiceIdProvider> _service_id_provider{ new ServiceIdProvider{} };
		std::map<uint16_t, uint16_t> _preset_pid_map;
		std::map<uint16_t, uint16_t> _preset_service_id_map;

	public:
		/// <summary>
		///		获取一个输入端，可以向此输入端输入一路 ts。每个 ts 必须独自占有一个输入端。
		/// 
		///		获取到输入端口后需要自行保管，如果丢失智能指针，将会无法找回，这会导致输出节目中原本属于该 ts 的流直接中断，
		///		再次调用 GetNewInputPort 获取输入端口后，也只是让输出多了几个 PID 的流而已，无法从丢失的流继续。
		/// </summary>
		/// <returns></returns>
		shared_ptr<ITsPacketConsumer> GetNewInputPort();

		#pragma region 通过 IPipeTsPacketSource 继承
		void AddTsPacketConsumer(shared_ptr<ITsPacketConsumer> packet_comsumer) override;
		bool RemovePacketComsumer(shared_ptr<ITsPacketConsumer> packet_comsumer) override;
		void ClearConsumers() override;
		#pragma endregion
	};
}