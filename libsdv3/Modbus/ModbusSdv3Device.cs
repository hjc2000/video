using JCNET;
using libsdv3.Code;
using libsdv3.Modbus.Frame;

namespace libsdv3.Modbus;

/// <summary>
///		利用 modbus 进行控制的 SDV3 设备。
/// </summary>
public class ModbusSdv3Device : IAsyncDisposable
{
	public ModbusSdv3Device(Stream serial_stream, byte device_addr, bool big_endian)
	{
		_serial_stream = serial_stream;
		_device_addr = device_addr;
		_big_endian = big_endian;
		_auto_bit_converter = new AutoBitConverter(big_endian);
	}

	private bool _disposed = false;
	public async ValueTask DisposeAsync()
	{
		if (_disposed)
		{
			return;
		}

		_disposed = true;
		GC.SuppressFinalize(this);

		await _serial_stream.DisposeAsync();
	}

	private byte _device_addr;
	private Stream _serial_stream;
	private bool _big_endian;
	private AutoBitConverter _auto_bit_converter;

	private static void PrintFrame(byte[] frame, bool is_send)
	{
		if (is_send)
		{
			Console.Write("发送：");
		}
		else
		{
			Console.Write("接收：");
		}

		foreach (byte b in frame)
		{
			Console.Write($"{b:x2} ");
		}

		Console.WriteLine();
	}

	#region modbus 基本操作函数
	/// <summary>
	///		检查 ADU。
	///		* 不包括 PDU 部分，只检查作为头部的地址和作为尾部的 CRC16。
	/// </summary>
	/// <param name="response_frame"></param>
	/// <exception cref="IOException"></exception>
	private void CheckADU(Memory<byte> response_frame)
	{
		ModbusCrc16 crc16 = new();
		crc16.Add(response_frame[..^2]);

		Span<byte> span = response_frame.Span;
		if (span[^2] != crc16.RegisterLowByte)
		{
			throw new IOException("CRC 校验错误");
		}

		if (span[^1] != crc16.RegisterHighByte)
		{
			throw new IOException("CRC 校验错误");
		}

		if (span[0] != _device_addr)
		{
			throw new IOException("接收到非预期地址的设备的响应");
		}
	}

	/// <summary>
	///		写单个位
	/// </summary>
	/// <param name="data_addr"></param>
	/// <param name="value"></param>
	/// <exception cref="IOException"></exception>
	private async Task WriteSingleBitAsync(ushort data_addr, bool value)
	{
		WriteSingleBitRequestFrame request_frame = new()
		{
			SlaveAddress = _device_addr,
			DataAddress = data_addr,
			BitValue = value,
		};
		byte[] frame = request_frame.ToBytes(_big_endian);
		PrintFrame(frame, true);
		await _serial_stream.WriteAsync(frame);

		// 接收
		byte[] read_buffer = new byte[8];
		await _serial_stream.ReadExactlyAsync(read_buffer);
		PrintFrame(read_buffer, false);
		CheckADU(read_buffer);
		if (read_buffer[1] != (byte)FunctionCode.WriteSingleBit)
		{
			throw new IOException("设备回复的帧中的功能码错误");
		}

		ushort received_data_addr = _auto_bit_converter.ToUInt16(read_buffer, 2);
		if (received_data_addr != data_addr)
		{
			throw new IOException("设备回复帧中的数据地址不对");
		}

		ushort received_data = _auto_bit_converter.ToUInt16(read_buffer, 4);
		if (received_data != 0 != value)
		{
			throw new IOException("设备回复帧中的数据不对");
		}
	}

	/// <summary>
	///		读取多个位数据。
	/// </summary>
	/// <param name="data_addr">数据地址</param>
	/// <param name="bit_count">要读取多少个位</param>
	/// <returns></returns>
	private async Task<byte[]> ReadBitsAsync(ushort data_addr, ushort bit_count)
	{
		if (bit_count == 0)
		{
			throw new ArgumentException("不允许读取 0 个位");
		}

		ReadBitsRequestFrame request_frame = new()
		{
			SlaveAddress = _device_addr,
			DataAddress = data_addr,
			BitCount = bit_count,
		};
		byte[] frame = request_frame.ToBytes(_big_endian);
		PrintFrame(frame, true);
		await _serial_stream.WriteAsync(frame);

		byte[] read_buffer = new byte[5 + (bit_count / 8) + 1];
		await _serial_stream.ReadExactlyAsync(read_buffer);
		PrintFrame(read_buffer, false);
		CheckADU(read_buffer);
		if (read_buffer[1] != (byte)FunctionCode.ReadBits)
		{
			throw new IOException("设备回复的帧中的功能码错误");
		}

		return read_buffer[3..^2];
	}

	/// <summary>
	///		读取各种数据
	/// </summary>
	/// <param name="data_addr">数据地址</param>
	/// <param name="record_count">记录数。一个记录是 16 位。读取 1 个 32 位的数据需要 2 个记录。</param>
	/// <returns></returns>
	private async Task<uint[]> ReadDatasAsync(ushort data_addr, ushort record_count)
	{
		if (record_count == 0)
		{
			throw new ArgumentException($"{nameof(record_count)} 不能为 0");
		}

		ReadDatasRequestFrame request_frame = new()
		{
			SlaveAddress = _device_addr,
			DataAddress = data_addr,
			RecordCount = record_count,
		};
		byte[] frame = request_frame.ToBytes(_big_endian);
		PrintFrame(frame, true);
		await _serial_stream.WriteAsync(frame);

		// 接收响应
		byte[] read_buffer = new byte[5 + (record_count * 2)];
		await _serial_stream.ReadExactlyAsync(read_buffer);
		PrintFrame(read_buffer, false);
		CheckADU(read_buffer);
		if (read_buffer[1] != (byte)FunctionCode.ReadDatas)
		{
			throw new IOException("设备回复的帧中的功能码错误");
		}

		if (read_buffer[2] != record_count * 2)
		{
			throw new IOException("返回的数据字节数不对");
		}

		int response_uint32_data_count = read_buffer[2] / 4;
		uint[] ret = new uint[response_uint32_data_count];
		for (int i = 0; i < response_uint32_data_count; i++)
		{
			int start_pos = 3 + (i * 4);
			ret[i] = _auto_bit_converter.ToUInt32(read_buffer, start_pos);
		}

		return ret;
	}

	private async Task WriteDatasAsync(ushort data_addr, uint[] datas)
	{
		if (datas.Length == 0)
		{
			throw new ArgumentException($"{nameof(datas)} 的长度不能为 0");
		}

		WriteDatasRequestFrame request_frame = new()
		{
			SlaveAddress = _device_addr,
			DataAddress = data_addr,
			Datas = datas,
		};
		byte[] frame = request_frame.ToBytes(_big_endian);
		PrintFrame(frame, true);
		await _serial_stream.WriteAsync(frame);

		// 接收响应
		byte[] read_buffer = new byte[8];
		await _serial_stream.ReadExactlyAsync(read_buffer);
		PrintFrame(read_buffer, false);
		CheckADU(read_buffer);
		if (read_buffer[1] != (byte)FunctionCode.WriteDatas)
		{
			throw new IOException("设备回复的帧中的功能码错误");
		}

		ushort response_data_addr = _auto_bit_converter.ToUInt16(read_buffer, 2);
		if (response_data_addr != data_addr)
		{
			throw new IOException("设备回复帧中的数据地址不对");
		}

		ushort response_record_count = _auto_bit_converter.ToUInt16(read_buffer, 4);
		if (response_record_count != datas.Length * 2)
		{
			throw new IOException("设备回复帧中的记录数不对");
		}
	}
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
