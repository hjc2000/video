using SDV3Lib.Code;

namespace SDV3Lib;

public interface ISdv3Device : IAsyncDisposable
{
	#region 硬件 EI
	Task<bool> GetEI1Async();
	Task<bool> GetEI2Async();
	Task<bool> GetEI3Async();
	Task<bool> GetEI4Async();
	Task<bool> GetEI5Async();
	#endregion

	#region 通信 EI
	Task<bool> GetEI9Async();
	Task SetEI9Async(bool value);

	Task<bool> GetEI10Async();
	Task SetEI10Async(bool value);

	Task<bool> GetEI11Async();
	Task SetEI11Async(bool value);

	Task<bool> GetEI12Async();
	Task SetEI12Async(bool value);

	Task<bool> GetEI13Async();
	Task SetEI13Async(bool value);

	Task<bool> GetEI14Async();
	Task SetEI14Async(bool value);

	Task<bool> GetEI15Async();
	Task SetEI15Async(bool value);

	Task<bool> GetEI16Async();
	Task SetEI16Async(bool value);

	Task<bool> GetEI17Async();
	Task SetEI17Async(bool value);

	Task<bool> GetEI18Async();
	Task SetEI18Async(bool value);

	Task<bool> GetEI19Async();
	Task SetEI19Async(bool value);

	Task<bool> GetEI20Async();
	Task SetEI20Async(bool value);

	Task<bool> GetEI21Async();
	Task SetEI21Async(bool value);

	Task<bool> GetEI22Async();
	Task SetEI22Async(bool value);

	Task<bool> GetEI23Async();
	Task SetEI23Async(bool value);

	Task<bool> GetEI24Async();
	Task SetEI24Async(bool value);
	#endregion

	#region EOUT
	Task<bool> GetEout1Async();
	Task<bool> GetEout2Async();
	Task<bool> GetEout3Async();
	Task<bool> GetEout6Async();
	Task<bool> GetEout7Async();
	Task<bool> GetEout8Async();
	Task<bool> GetEout9Async();
	Task<bool> GetEout10Async();
	Task<bool> GetEout11Async();
	Task<bool> GetEout12Async();
	Task<bool> GetEout13Async();
	Task<bool> GetEout14Async();
	Task<bool> GetEout15Async();
	Task<bool> GetEout16Async();
	Task<bool> GetEout17Async();
	Task<bool> GetEout18Async();
	Task<bool> GetEout19Async();
	Task<bool> GetEout20Async();
	Task<bool> GetEout21Async();
	#endregion

	#region 监控数据
	Task<int> GetFeedbackSpeedAsync();
	Task<uint> GetCommandSpeedAsync();
	Task<uint> GetCommandTorqueAsync();
	Task<uint> GetPeakTorqueAsync();
	Task<uint> GetMotorCurrentAsync();
	Task<uint> GetEffectiveTorqueAsync();
	Task<int> GetFeedbackCurrentPositionAsync();
	Task<uint> GetCommandCurrentPositionAsync();
	Task<uint> GetPositionDeviationAsync();
	Task<uint> GetCommandPulseFrequencyAsync();
	Task<uint> GetTotalFeedbackPulseCountAsync();
	Task<uint> GetTotalCommandPulseCountAsync();
	Task<uint> GetPulseCountBetween_LS_And_Z_Async();
	Task<uint> GetLoadInertiaTorqueRatioAsync();
	Task<uint> GetMaxDCIntermediateVoltageAsync();
	Task<uint> GetMinDCIntermediateVoltageAsync();
	Task<uint> GetVrefInputVoltageAsync();
	Task<uint> GetTrefInputVoltageAsync();
	Task<uint> GetOlForecastAsync();
	#endregion

	#region 报警
	Task<AlarmCode> GetCurrentAlarmAsync();
	Task<AlarmCode> GetAlarmRecordsAsync(int record_id);
	public int AlarmRecordCount { get; }
	#endregion

	/// <summary>
	///		控制模式
	/// </summary>
	Task<ControlModeCode> GetControlModeAsync();

	/// <summary>
	///		动作模式
	/// </summary>
	Task<ActionModeCode> GetActionModeAsync();

	Task<uint> GetPnAsync(int major, int minor);
	Task SetPnAsync(int major, int minor, uint[] value);
	Task SetPnAsync(int major, int minor, uint value);

	#region 立即数
	Task<uint> GetImmediateStateAsync();
	Task SetImmediateStateAsync(uint value);

	Task<int> GetImmediatePositionAsync();
	Task SetImmediatePositionAsync(int value);

	Task<int> GetImmediateSpeedAsync();
	Task SetImmediateSpeedAsync(int value);

	Task<int> GetImmediateAccelerationDurationAsync();
	Task SetImmediateAccelerationDurationAsync(int value);

	Task<int> GetImmediateDecelerationDurationAsync();
	Task SetImmediateDecelerationDurationAsync(int value);
	#endregion

	Task<int> GetSpeedAsync();
	Task SetSpeedAsync(int value);
}
