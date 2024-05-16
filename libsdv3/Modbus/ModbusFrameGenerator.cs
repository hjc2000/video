using JCNET;

namespace libsdv3.Modbus;

public static class ModbusFrameGenerator
{
	public static bool Bigendian { get; set; } = true;

	public static byte[] ReadBits(byte device_addr, ushort data_addr, ushort bit_count)
	{
		byte[] frame = new byte[6];
		frame[0] = device_addr;
		frame[1] = (byte)FunctionCode.ReadBits;
		if (Bigendian && BitConverter.IsLittleEndian)
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

	public static byte[] WriteSingleBit(byte device_addr, ushort data_addr, bool value)
	{
		byte[] frame = new byte[6];
		ushort data = value ? (ushort)0Xff00 : (ushort)0;

		frame[0] = device_addr;
		frame[1] = (byte)FunctionCode.WriteSingleBit;
		if (Bigendian && BitConverter.IsLittleEndian)
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
}
