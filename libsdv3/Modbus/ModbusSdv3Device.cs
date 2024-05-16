using JCNET;
using System.IO.Ports;

namespace libsdv3.Modbus;

/// <summary>
///		利用 modbus 进行控制的 SDV3 设备。
/// </summary>
public class ModbusSdv3Device : ISdv3Device, IModbusDevice
{
	public ModbusSdv3Device(SerialPort serial, byte device_addr)
	{
		_serial = serial;
		_device_addr = device_addr;
	}

	private byte _device_addr = 1;
	private SerialPort _serial;

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

	private static ushort ToLocalEndian(byte[] buffer)
	{
		if (buffer.Length != 2)
		{
			throw new ArgumentException($"{nameof(buffer)}必须是长度为 2 的数组");
		}

		if (BitConverter.IsLittleEndian ^ !ModbusFrameGenerator.Bigendian)
		{
			Array.Reverse(buffer);
		}

		return BitConverter.ToUInt16(buffer, 0);
	}

	public void WriteSingleBit(ushort data_addr, bool value)
	{
		_serial.Write(ModbusFrameGenerator.WriteSingleBit(_device_addr, data_addr, value));
		byte[] read_buffer = _serial.ReadExactly(8);
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

	public bool EI9
	{
		get
		{
			lock (this)
			{
				_serial.Write(ModbusFrameGenerator.ReadBits(_device_addr, 0x0208, 1));
				byte[] read_buffer = _serial.ReadExactly(6);
				CheckADU(read_buffer);
				if (read_buffer[1] != (byte)FunctionCode.ReadBits)
				{
					throw new IOException("设备回复的帧中的功能码错误");
				}

				return read_buffer[^3] != 0;
			}
		}
		set
		{
			lock (this)
			{
				WriteSingleBit(0x0208, value);
			}
		}
	}
}
