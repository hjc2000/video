using JCNET.Modbus;
using libsdv3.Code;

namespace libsdv3.Modbus;

public interface IModbusSdv3Device : IAsyncDisposable
{
	#region modbus 基本操作函数
	/// <summary>
	///		写单个位
	/// </summary>
	/// <param name="data_addr"></param>
	/// <param name="value"></param>
	/// <exception cref="ModbusFrameException"></exception>
	Task WriteSingleBitAsync(ushort data_addr, bool value);

	/// <summary>
	///		读取多个位数据。
	/// </summary>
	/// <param name="data_addr">数据地址</param>
	/// <param name="bit_count">要读取多少个位</param>
	/// <returns></returns>
	/// <exception cref="ModbusFrameException"></exception>
	Task<byte[]> ReadBitsAsync(ushort data_addr, ushort bit_count);

	/// <summary>
	///		读取各种数据
	/// </summary>
	/// <param name="data_addr">数据地址</param>
	/// <param name="record_count">记录数。一个记录是 16 位。读取 1 个 32 位的数据需要 2 个记录。</param>
	/// <returns></returns>
	/// <exception cref="ModbusFrameException"></exception>
	Task<uint[]> ReadDatasAsync(ushort data_addr, ushort record_count);

	/// <summary>
	///		写各种数据
	/// </summary>
	/// <param name="data_addr"></param>
	/// <param name="datas"></param>
	/// <returns></returns>
	/// <exception cref="ModbusFrameException"></exception>
	Task WriteDatasAsync(ushort data_addr, uint[] datas);
	#endregion

	#region 硬件 EI
	public async Task<bool> GetEI1Async()
	{
		Console.WriteLine("读取 EI1");
		byte[] bits = await ReadBitsAsync(ParamAddress.EI1, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEI2Async()
	{
		Console.WriteLine("读取 EI2");
		byte[] bits = await ReadBitsAsync(ParamAddress.EI2, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEI3Async()
	{
		Console.WriteLine("读取 EI3");
		byte[] bits = await ReadBitsAsync(ParamAddress.EI3, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEI4Async()
	{
		Console.WriteLine("读取 EI4");
		byte[] bits = await ReadBitsAsync(ParamAddress.EI4, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEI5Async()
	{
		Console.WriteLine("读取 EI5");
		byte[] bits = await ReadBitsAsync(ParamAddress.EI5, 1);
		return bits[0] != 0;
	}
	#endregion

	#region 通信 EI
	public async Task<bool> GetEI9Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI9, 1);
		return bits[0] != 0;
	}

	public async Task SetEI9Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI9, value);
	}

	public async Task<bool> GetEI10Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI10, 1);
		return bits[0] != 0;
	}

	public async Task SetEI10Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI10, value);
	}

	public async Task<bool> GetEI11Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI11, 1);
		return bits[0] != 0;
	}

	public async Task SetEI11Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI11, value);
	}

	public async Task<bool> GetEI12Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI12, 1);
		return bits[0] != 0;
	}

	public async Task SetEI12Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI12, value);
	}

	public async Task<bool> GetEI13Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI13, 1);
		return bits[0] != 0;
	}

	public async Task SetEI13Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI13, value);
	}

	public async Task<bool> GetEI14Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI14, 1);
		return bits[0] != 0;
	}

	public async Task SetEI14Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI14, value);
	}

	public async Task<bool> GetEI15Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI15, 1);
		return bits[0] != 0;
	}

	public async Task SetEI15Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI15, value);
	}

	public async Task<bool> GetEI16Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI16, 1);
		return bits[0] != 0;
	}

	public async Task SetEI16Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI16, value);
	}

	public async Task<bool> GetEI17Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI17, 1);
		return bits[0] != 0;
	}

	public async Task SetEI17Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI17, value);
	}

	public async Task<bool> GetEI18Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI18, 1);
		return bits[0] != 0;
	}

	public async Task SetEI18Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI18, value);
	}

	public async Task<bool> GetEI19Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI19, 1);
		return bits[0] != 0;
	}

	public async Task SetEI19Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI19, value);
	}

	public async Task<bool> GetEI20Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI20, 1);
		return bits[0] != 0;
	}

	public async Task SetEI20Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI20, value);
	}

	public async Task<bool> GetEI21Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI21, 1);
		return bits[0] != 0;
	}

	public async Task SetEI21Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI21, value);
	}

	public async Task<bool> GetEI22Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI22, 1);
		return bits[0] != 0;
	}

	public async Task SetEI22Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI22, value);
	}

	public async Task<bool> GetEI23Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI23, 1);
		return bits[0] != 0;
	}

	public async Task SetEI23Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI23, value);
	}

	public async Task<bool> GetEI24Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EI24, 1);
		return bits[0] != 0;
	}

	public async Task SetEI24Async(bool value)
	{
		await WriteSingleBitAsync(ParamAddress.EI24, value);
	}
	#endregion

	#region EOUT
	public async Task<bool> GetEout1Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT1, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout2Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT2, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout3Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT3, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout6Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT6, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout7Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT7, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout8Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT8, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout9Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT9, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout10Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT10, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout11Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT11, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout12Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT12, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout13Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT13, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout14Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT14, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout15Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT15, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout16Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT16, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout17Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT17, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout18Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT18, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout19Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT19, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout20Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT20, 1);
		return bits[0] != 0;
	}

	public async Task<bool> GetEout21Async()
	{
		byte[] bits = await ReadBitsAsync(ParamAddress.EOUT21, 1);
		return bits[0] != 0;
	}
	#endregion

	#region 监控数据
	public async Task<int> GetFeedbackSpeedAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.FeedbackSpeed, 2);
		return (int)datas[0];
	}

	public async Task<uint> GetCommandSpeedAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.CommandSpeed, 2);
		return datas[0];
	}

	public async Task<uint> GetCommandTorqueAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.CommandTorque, 2);
		return datas[0];
	}

	public async Task<uint> GetPeakTorqueAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.PeakTorque, 2);
		return datas[0];
	}

	public async Task<uint> GetMotorCurrentAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.MotorCurrent, 2);
		return datas[0];
	}

	public async Task<uint> GetEffectiveTorqueAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.EffectiveTorque, 2);
		return datas[0];
	}

	public async Task<int> GetFeedbackCurrentPositionAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.FeedbackCurrentPosition, 2);
		return (int)datas[0];
	}

	public async Task<uint> GetCommandCurrentPositionAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.CommandCurrentPosition, 2);
		return datas[0];
	}

	public async Task<uint> GetPositionDeviationAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.PositionDeviation, 2);
		return datas[0];
	}

	public async Task<uint> GetCommandPulseFrequencyAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.CommandPulseFrequency, 2);
		return datas[0];
	}

	public async Task<uint> GetTotalFeedbackPulseCountAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.TotalFeedbackPulseCount, 2);
		return datas[0];
	}

	public async Task<uint> GetTotalCommandPulseCountAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.TotalCommandPulseCount, 2);
		return datas[0];
	}

	public async Task<uint> GetPulseCountBetween_LS_And_Z_Async()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.PulseCountBetween_LS_And_Z, 2);
		return datas[0];
	}

	public async Task<uint> GetLoadInertiaTorqueRatioAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.LoadInertiaTorqueRatio, 2);
		return datas[0];
	}

	public async Task<uint> GetMaxDCIntermediateVoltageAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.MaxDCIntermediateVoltage, 2);
		return datas[0];
	}

	public async Task<uint> GetMinDCIntermediateVoltageAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.MinDCIntermediateVoltage, 2);
		return datas[0];
	}

	public async Task<uint> GetVrefInputVoltageAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.VrefInputVoltage, 2);
		return datas[0];
	}

	public async Task<uint> GetTrefInputVoltageAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.TrefInputVoltage, 2);
		return datas[0];
	}

	public async Task<uint> GetOlForecastAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.OLForecast, 2);
		return datas[0];
	}
	#endregion

	#region 报警
	public async Task<AlarmCode> GetCurrentAlarmAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.CurrentAlarm, 2);
		return (AlarmCode)datas[0];
	}

	public async Task<AlarmCode> GetAlarmRecordsAsync(int record_id)
	{
		if (record_id < 1 || record_id > AlarmRecordCount)
		{
			throw new ArgumentOutOfRangeException($"{nameof(record_id)} 必须在 [1, {AlarmRecordCount}] 上。");
		}

		uint[] datas = await ReadDatasAsync(ParamAddress.AlarmRecords(record_id), 2);
		return (AlarmCode)datas[0];
	}

	public int AlarmRecordCount
	{
		get
		{
			return 20;
		}
	}
	#endregion

	/// <summary>
	///		控制模式
	/// </summary>
	public async Task<ControlModeCode> GetControlModeAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.CurrentAlarm, 2);
		return (ControlModeCode)datas[0];
	}

	/// <summary>
	///		动作模式
	/// </summary>
	public async Task<ActionModeCode> GetActionModeAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.CurrentAlarm, 2);
		return (ActionModeCode)datas[0];
	}

	public async Task<uint> GetPnAsync(int major, int minor)
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.Pn(major, minor), 2);
		return datas[0];
	}

	public async Task SetPnAsync(int major, int minor, uint[] value)
	{
		await WriteDatasAsync(ParamAddress.Pn(major, minor), value);
	}

	public async Task SetPnAsync(int major, int minor, uint value)
	{
		await SetPnAsync(major, minor, [value]);
	}

	#region 立即数
	public async Task<int> GetImmediatePositionAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.ImmediatePosition, 2);
		return (int)datas[0];
	}

	public async Task SetImmediatePositionAsync(int value)
	{
		await WriteDatasAsync(ParamAddress.ImmediatePosition, [(uint)value]);
	}

	public async Task<int> GetImmediateSpeedAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.ImmediateSpeed, 2);
		return (int)datas[0];
	}

	public async Task SetImmediateSpeedAsync(int value)
	{
		await WriteDatasAsync(ParamAddress.ImmediateSpeed, [(uint)value]);
	}

	public async Task<int> GetImmediateAccelerationDurationAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.ImmediateAccelerationDuration, 2);
		return (int)datas[0];
	}

	public async Task SetImmediateAccelerationDuration(int value)
	{
		await WriteDatasAsync(ParamAddress.ImmediateAccelerationDuration, [(uint)value]);
	}

	public async Task<int> GetImmediateDecelerationDuration()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.ImmediateDecelerationDuration, 2);
		return (int)datas[0];
	}

	public async Task SetImmediateDecelerationDuration(int value)
	{
		await WriteDatasAsync(ParamAddress.ImmediateDecelerationDuration, [(uint)value]);
	}
	#endregion

	public async Task<int> GetSpeedAsync()
	{
		uint[] datas = await ReadDatasAsync(ParamAddress.Speed, 2);
		return (int)datas[0];
	}

	public async Task SetSpeedAsync(int value)
	{
		await WriteDatasAsync(ParamAddress.Speed, [(uint)value]);
	}
}
