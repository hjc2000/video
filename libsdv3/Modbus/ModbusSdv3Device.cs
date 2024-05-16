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
	///		对接收到的帧进行 CRC16
	/// </summary>
	/// <param name="received_frame"></param>
	/// <exception cref="IOException"></exception>
	private void EnsureReceivedFrameCrc16(byte[] received_frame)
	{
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

	public bool EI9
	{
		get
		{
			lock (this)
			{
				_serial.Write(ModbusFrameGenerator.ReadBits(_device_addr, 0x0208, 1));
				byte[] read_buffer = _serial.ReadExactly(6);
				if (read_buffer[0] != _device_addr)
				{
					throw new IOException("接收到非预期地址的设备的响应");
				}

				if (read_buffer[1] != (byte)FunctionCode.ReadBits)
				{
					throw new IOException("设备回复的帧中的功能码错误");
				}

				EnsureReceivedFrameCrc16(read_buffer);
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

	public void WriteSingleBit(ushort addr, bool value)
	{
		_serial.Write(ModbusFrameGenerator.WriteSingleBit(_device_addr, addr, value));
		byte[] read_buffer = _serial.ReadExactly(8);
		if (read_buffer[0] != _device_addr)
		{
			throw new IOException("接收到非预期地址的设备的响应");
		}

		if (read_buffer[1] != (byte)FunctionCode.WriteSingleBit)
		{
			throw new IOException("设备回复的帧中的功能码错误");
		}

		EnsureReceivedFrameCrc16(read_buffer);
	}
}
