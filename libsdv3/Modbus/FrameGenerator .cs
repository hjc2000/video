using JCNET;

namespace libsdv3.Modbus;

public enum FunctionCode
{
	ReadBits = 0x01,
}

public static class FrameGenerator
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
		return [.. frame, crc16.Crc16RegisterValueLowByte, crc16.Crc16RegisterValueHighByte];
	}
}
