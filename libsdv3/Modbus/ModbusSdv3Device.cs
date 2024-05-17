using JCNET;
using System.IO.Ports;

namespace libsdv3.Modbus;

/// <summary>
///		利用 modbus 进行控制的 SDV3 设备。
/// </summary>
public class ModbusSdv3Device : ISdv3Device
{
	public ModbusSdv3Device(SerialPort serial, byte device_addr)
	{
		_serial = serial;
		_device_addr = device_addr;
	}

	private byte _device_addr = 1;
	private SerialPort _serial;

	private bool Bigendian { get; set; } = true;

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
	/// <param name="received_frame"></param>
	/// <exception cref="IOException"></exception>
	private void CheckADU(byte[] received_frame)
	{
		if (received_frame[0] != _device_addr)
		{
			throw new IOException("接收到非预期地址的设备的响应");
		}

		ModbusCrc16 crc16 = new();
		crc16.Add(received_frame[..^2]);
		if (received_frame[^2] != crc16.RegisterLowByte)
		{
			throw new IOException("CRC 校验错误");
		}

		if (received_frame[^1] != crc16.RegisterHighByte)
		{
			throw new IOException("CRC 校验错误");
		}
	}

	private ushort ToLocalEndian(byte[] buffer)
	{
		if (buffer.Length != 2)
		{
			throw new ArgumentException($"{nameof(buffer)}必须是长度为 2 的数组");
		}

		if (BitConverter.IsLittleEndian ^ !Bigendian)
		{
			Array.Reverse(buffer);
		}

		return BitConverter.ToUInt16(buffer, 0);
	}

	private void WriteSingleBit(ushort data_addr, bool value)
	{
		byte[] GenerateWriteSingleBitFrame(ushort data_addr, bool value)
		{
			byte[] frame = new byte[6];
			ushort data = value ? (ushort)0Xff00 : (ushort)0;

			frame[0] = _device_addr;
			frame[1] = (byte)FunctionCode.WriteSingleBit;
			if (Bigendian)
			{
				// 如果数据要求大端序，并且本机是小端序
				frame[2] = (byte)(data_addr >> 8);
				frame[3] = (byte)data_addr;

				frame[4] = (byte)(data >> 8);
				frame[5] = (byte)data;
			}
			else
			{
				frame[2] = (byte)data_addr;
				frame[3] = (byte)(data_addr >> 8);

				frame[4] = (byte)data;
				frame[5] = (byte)(data >> 8);
			}

			ModbusCrc16 crc16 = new();
			crc16.Add(frame);
			return [.. frame, crc16.RegisterLowByte, crc16.RegisterHighByte];
		}

		byte[] frame = GenerateWriteSingleBitFrame(data_addr, value);
		PrintFrame(frame, true);
		_serial.Write(frame);

		byte[] read_buffer = _serial.ReadExactly(8);
		PrintFrame(read_buffer, false);
		CheckADU(read_buffer);
		if (read_buffer[1] != (byte)FunctionCode.WriteSingleBit)
		{
			throw new IOException("设备回复的帧中的功能码错误");
		}

		byte[] temp_buffer = read_buffer[2..4];
		ushort received_data_addr = ToLocalEndian(temp_buffer);
		if (received_data_addr != data_addr)
		{
			throw new IOException("设备回复帧中的数据地址不对");
		}

		temp_buffer = read_buffer[4..6];
		ushort received_data = ToLocalEndian(temp_buffer);
		if (received_data != 0 != value)
		{
			throw new IOException("设备回复帧中的数据不对");
		}
	}

	/// <summary>
	///		读取多个位数据。
	/// </summary>
	/// <param name="data_addr">数据地址</param>
	/// <param name="count">要读取多少个位</param>
	/// <returns></returns>
	private byte[] ReadBits(ushort data_addr, ushort count)
	{
		if (count == 0)
		{
			throw new ArgumentException("不允许读取 0 个位");
		}

		byte[] GenerateReadBitsFrame(ushort data_addr, ushort bit_count)
		{
			byte[] frame = new byte[6];
			frame[0] = _device_addr;
			frame[1] = (byte)FunctionCode.ReadBits;
			if (Bigendian)
			{
				// 如果数据要求大端序，并且本机是小端序
				frame[2] = (byte)(data_addr >> 8);
				frame[3] = (byte)data_addr;

				frame[4] = (byte)(bit_count >> 8);
				frame[5] = (byte)bit_count;
			}
			else
			{
				frame[2] = (byte)data_addr;
				frame[3] = (byte)(data_addr >> 8);

				frame[4] = (byte)bit_count;
				frame[5] = (byte)(bit_count >> 8);
			}

			ModbusCrc16 crc16 = new();
			crc16.Add(frame);
			return [.. frame, crc16.RegisterLowByte, crc16.RegisterHighByte];
		}

		byte[] frame = GenerateReadBitsFrame(data_addr, 1);
		PrintFrame(frame, true);
		_serial.Write(frame);

		byte[] read_buffer = _serial.ReadExactly(5 + (count / 8) + 1);
		PrintFrame(read_buffer, false);
		CheckADU(read_buffer);
		if (read_buffer[1] != (byte)FunctionCode.ReadBits)
		{
			throw new IOException("设备回复的帧中的功能码错误");
		}

		return read_buffer[3..^2];
	}

	public bool EI9
	{
		get
		{
			lock (this)
			{
				Console.WriteLine("读取 EI9");
				byte[] bits = ReadBits(0x0208, 1);
				return bits[0] != 0;
			}
		}
		set
		{
			lock (this)
			{
				Console.WriteLine("写入 EI9");
				WriteSingleBit(0x0208, value);
			}
		}
	}
}
