using JCNET;
using libsdv3.Modbus.Frame;

namespace libsdv3.Modbus;

/// <summary>
///		利用 modbus 进行控制的 SDV3 设备。
/// </summary>
public partial class ModbusSdv3Device : ISdv3Device
{
	public ModbusSdv3Device(Stream serial_stream, byte device_addr, bool big_endian)
	{
		_serial_stream = serial_stream;
		_device_addr = device_addr;
		_big_endian = big_endian;
		_auto_bit_converter = new AutoBitConverter(big_endian);
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
	private void WriteSingleBit(ushort data_addr, bool value)
	{
		WriteSingleBitRequestFrame request_frame = new()
		{
			SlaveAddress = _device_addr,
			DataAddress = data_addr,
			BitValue = value,
		};
		byte[] frame = request_frame.ToBytes(_big_endian);
		PrintFrame(frame, true);
		_serial_stream.Write(frame);

		byte[] read_buffer = new byte[8];
		_serial_stream.ReadExactly(read_buffer);
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
	private byte[] ReadBits(ushort data_addr, ushort bit_count)
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
		_serial_stream.Write(frame);

		byte[] read_buffer = new byte[5 + (bit_count / 8) + 1];
		_serial_stream.ReadExactly(read_buffer);
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
	private uint[] ReadDatas(ushort data_addr, ushort record_count)
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
		_serial_stream.Write(frame);

		// 接收响应
		byte[] read_buffer = new byte[5 + (record_count * 2)];
		_serial_stream.ReadExactly(read_buffer);
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

	private void WriteDatas(ushort data_addr, uint[] datas)
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
		_serial_stream.Write(frame);

		// 接收响应
		byte[] read_buffer = new byte[8];
		_serial_stream.ReadExactly(read_buffer);
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
}

public partial class ModbusSdv3Device
{
	#region 硬件 EI
	public bool EI1
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI1");
				byte[] bits = ReadBits(ParamAddress.EI1, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EI2
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI2");
				byte[] bits = ReadBits(ParamAddress.EI2, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EI3
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI3");
				byte[] bits = ReadBits(ParamAddress.EI3, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EI4
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI4");
				byte[] bits = ReadBits(ParamAddress.EI4, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EI5
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI5");
				byte[] bits = ReadBits(ParamAddress.EI5, 1);
				return bits[0] != 0;
			}
		}
	}
	#endregion

	#region 通信 EI
	public bool EI9
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI9");
				byte[] bits = ReadBits(ParamAddress.EI9, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI9");
				WriteSingleBit(ParamAddress.EI9, value);
			}
		}
	}

	public bool EI10
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI10");
				byte[] bits = ReadBits(ParamAddress.EI10, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI10");
				WriteSingleBit(ParamAddress.EI10, value);
			}
		}
	}

	public bool EI11
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI11");
				byte[] bits = ReadBits(ParamAddress.EI11, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI11");
				WriteSingleBit(ParamAddress.EI11, value);
			}
		}
	}

	public bool EI12
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI12");
				byte[] bits = ReadBits(ParamAddress.EI12, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI12");
				WriteSingleBit(ParamAddress.EI12, value);
			}
		}
	}

	public bool EI13
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI13");
				byte[] bits = ReadBits(ParamAddress.EI13, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI13");
				WriteSingleBit(ParamAddress.EI13, value);
			}
		}
	}

	public bool EI14
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI14");
				byte[] bits = ReadBits(ParamAddress.EI14, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI14");
				WriteSingleBit(ParamAddress.EI14, value);
			}
		}
	}

	public bool EI15
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI15");
				byte[] bits = ReadBits(ParamAddress.EI15, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI15");
				WriteSingleBit(ParamAddress.EI15, value);
			}
		}
	}

	public bool EI16
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI16");
				byte[] bits = ReadBits(ParamAddress.EI16, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI16");
				WriteSingleBit(ParamAddress.EI16, value);
			}
		}
	}

	public bool EI17
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI17");
				byte[] bits = ReadBits(ParamAddress.EI17, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI17");
				WriteSingleBit(ParamAddress.EI17, value);
			}
		}
	}

	public bool EI18
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI18");
				byte[] bits = ReadBits(ParamAddress.EI18, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI18");
				WriteSingleBit(ParamAddress.EI18, value);
			}
		}
	}

	public bool EI19
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI19");
				byte[] bits = ReadBits(ParamAddress.EI19, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI19");
				WriteSingleBit(ParamAddress.EI19, value);
			}
		}
	}

	public bool EI20
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI20");
				byte[] bits = ReadBits(ParamAddress.EI20, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI20");
				WriteSingleBit(ParamAddress.EI20, value);
			}
		}
	}

	public bool EI21
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI21");
				byte[] bits = ReadBits(ParamAddress.EI21, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI21");
				WriteSingleBit(ParamAddress.EI21, value);
			}
		}
	}

	public bool EI22
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI22");
				byte[] bits = ReadBits(ParamAddress.EI22, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI22");
				WriteSingleBit(ParamAddress.EI22, value);
			}
		}
	}

	public bool EI23
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI23");
				byte[] bits = ReadBits(ParamAddress.EI23, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI23");
				WriteSingleBit(ParamAddress.EI23, value);
			}
		}
	}

	public bool EI24
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI24");
				byte[] bits = ReadBits(ParamAddress.EI24, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI24");
				WriteSingleBit(ParamAddress.EI24, value);
			}
		}
	}
	#endregion

	#region EOUT
	public bool EOUT1
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT1");
				byte[] bits = ReadBits(ParamAddress.EOUT1, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT2
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT2");
				byte[] bits = ReadBits(ParamAddress.EOUT2, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT3
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT3");
				byte[] bits = ReadBits(ParamAddress.EOUT3, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT6
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT6");
				byte[] bits = ReadBits(ParamAddress.EOUT6, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT7
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT7");
				byte[] bits = ReadBits(ParamAddress.EOUT7, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT8
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT8");
				byte[] bits = ReadBits(ParamAddress.EOUT8, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT9
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT9");
				byte[] bits = ReadBits(ParamAddress.EOUT9, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT10
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT10");
				byte[] bits = ReadBits(ParamAddress.EOUT10, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT11
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT11");
				byte[] bits = ReadBits(ParamAddress.EOUT11, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT12
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT12");
				byte[] bits = ReadBits(ParamAddress.EOUT12, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT13
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT13");
				byte[] bits = ReadBits(ParamAddress.EOUT13, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT14
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT14");
				byte[] bits = ReadBits(ParamAddress.EOUT14, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT15
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT15");
				byte[] bits = ReadBits(ParamAddress.EOUT15, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT16
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT16");
				byte[] bits = ReadBits(ParamAddress.EOUT16, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT17
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT17");
				byte[] bits = ReadBits(ParamAddress.EOUT17, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT18
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT18");
				byte[] bits = ReadBits(ParamAddress.EOUT18, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT19
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT19");
				byte[] bits = ReadBits(ParamAddress.EOUT19, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT20
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT20");
				byte[] bits = ReadBits(ParamAddress.EOUT20, 1);
				return bits[0] != 0;
			}
		}
	}

	public bool EOUT21
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EOUT21");
				byte[] bits = ReadBits(ParamAddress.EOUT21, 1);
				return bits[0] != 0;
			}
		}
	}
	#endregion

	#region 监控数据
	public uint FeedbackSpeed
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.FeedbackSpeed, 2);
			return datas[0];
		}
	}

	public uint CommandSpeed
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.CommandSpeed, 2);
			return datas[0];
		}
	}

	public uint CommandTorque
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.CommandTorque, 2);
			return datas[0];
		}
	}

	public uint PeakTorque
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.PeakTorque, 2);
			return datas[0];
		}
	}

	public uint MotorCurrent
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.MotorCurrent, 2);
			return datas[0];
		}
	}

	public uint EffectiveTorque
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.EffectiveTorque, 2);
			return datas[0];
		}
	}

	public uint FeedbackCurrentPosition
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.FeedbackCurrentPosition, 2);
			return datas[0];
		}
	}

	public uint CommandCurrentPosition
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.CommandCurrentPosition, 2);
			return datas[0];
		}
	}

	public uint PositionDeviation
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.PositionDeviation, 2);
			return datas[0];
		}
	}

	public uint CommandPulseFrequency
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.CommandPulseFrequency, 2);
			return datas[0];
		}
	}

	public uint TotalFeedbackPulseCount
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.TotalFeedbackPulseCount, 2);
			return datas[0];
		}
	}

	public uint TotalCommandPulseCount
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.TotalCommandPulseCount, 2);
			return datas[0];
		}
	}

	public uint PulseCountBetween_LS_And_Z
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.PulseCountBetween_LS_And_Z, 2);
			return datas[0];
		}
	}

	public uint LoadInertiaTorqueRatio
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.LoadInertiaTorqueRatio, 2);
			return datas[0];
		}
	}

	public uint MaxDCIntermediateVoltage
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.MaxDCIntermediateVoltage, 2);
			return datas[0];
		}
	}

	public uint MinDCIntermediateVoltage
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.MinDCIntermediateVoltage, 2);
			return datas[0];
		}
	}

	public uint VrefInputVoltage
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.VrefInputVoltage, 2);
			return datas[0];
		}
	}

	public uint TrefInputVoltage
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.TrefInputVoltage, 2);
			return datas[0];
		}
	}

	public uint OLForecast
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.OLForecast, 2);
			return datas[0];
		}
	}
	#endregion

	public uint CurrentAlarm
	{
		get
		{
			uint[] datas = ReadDatas(ParamAddress.CurrentAlarm, 2);
			return datas[0];
		}
	}

	public uint AlarmRecords(int record_id)
	{
		if (record_id < 1 || record_id > AlarmRecordCount)
		{
			throw new ArgumentOutOfRangeException($"{nameof(record_id)} 必须在 [1, {AlarmRecordCount}] 上。");
		}

		uint[] datas = ReadDatas(ParamAddress.AlarmRecords(record_id), 2);
		return datas[0];
	}

	public int AlarmRecordCount
	{
		get
		{
			return 20;
		}
	}

	public uint Pn(int major, int minor)
	{
		uint[] datas = ReadDatas(ParamAddress.Pn(major, minor), 2);
		return datas[0];
	}
	public void SetPn(int major, int minor, uint[] value)
	{
		WriteDatas(ParamAddress.Pn(major, minor), value);
	}
	public void SetPn(int major, int minor, uint value)
	{
		WriteDatas(ParamAddress.Pn(major, minor), [value]);
	}
}
