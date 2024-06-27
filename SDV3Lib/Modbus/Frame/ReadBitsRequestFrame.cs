using JCNET;
using JCNET.Modbus;

namespace SDV3Lib.Modbus.Frame;

public class ReadBitsRequestFrame
{
	public byte SlaveAddress { get; set; } = 1;
	private FunctionCode FunctionCode { get; } = FunctionCode.ReadBits;
	public ushort DataAddress { get; set; } = 0;
	public ushort BitCount { get; set; } = 1;

	public byte[] ToBytes(bool big_endian)
	{
		AutoBitConverter auto_bit_converter = new(big_endian);
		byte[] frame = new byte[8];

		frame[0] = SlaveAddress;
		frame[1] = (byte)FunctionCode;

		byte[] data_addr_bytes = auto_bit_converter.GetBytes(DataAddress);
		frame[2] = data_addr_bytes[0];
		frame[3] = data_addr_bytes[1];

		byte[] data_bytes = auto_bit_converter.GetBytes(BitCount);
		frame[4] = data_bytes[0];
		frame[5] = data_bytes[1];

		ModbusCrc16 crc16 = new();
		crc16.Add(new Memory<byte>(frame, 0, 6));
		frame[6] = crc16.RegisterLowByte;
		frame[7] = crc16.RegisterHighByte;
		return frame;
	}
}
