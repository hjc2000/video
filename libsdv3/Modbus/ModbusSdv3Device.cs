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

	private SerialPort _serial;
	private byte _device_addr = 1;
	private ModbusCrc16 _crc16 = new();

	public bool EI9
	{
		get
		{
			byte[] send_buffer = FrameGenerator.ReadBits(_device_addr, 0x0208, 1);
			_serial.Write(send_buffer, 0, send_buffer.Length);

			byte[] read_buffer = new byte[6];
			int total_read = 0;
			while (total_read < read_buffer.Length)
			{
				int have_read = _serial.Read(read_buffer, total_read, read_buffer.Length - total_read);
				total_read += have_read;
			}

			if (read_buffer[0] != _device_addr)
			{
				throw new IOException("接收到非预期地址的设备的响应");
			}

			if (read_buffer[1] != (byte)FunctionCode.ReadBits)
			{
				throw new IOException("设备回复的帧中的功能码错误");
			}

			ModbusCrc16 crc16 = new();
			crc16.Add(read_buffer[..^2]);
			if (!(read_buffer[^1] == crc16.Crc16RegisterValueHighByte && read_buffer[^2] == crc16.Crc16RegisterValueLowByte))
			{
				throw new IOException("CRC 校验错误");
			}

			return read_buffer[^3] != 0;
		}
		set
		{

		}
	}
}
