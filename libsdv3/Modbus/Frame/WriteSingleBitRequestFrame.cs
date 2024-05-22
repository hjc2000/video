using JCNET;
using JCNET.Modbus;

namespace libsdv3.Modbus.Frame;

/// <summary>
///		写单个位的请求帧。由主机发给从机。
/// </summary>
public class WriteSingleBitRequestFrame
{
	/// <summary>
	///		从机地址
	/// </summary>
	public byte SlaveAddress { get; set; } = 1;

	private FunctionCode FunctionCode { get; } = FunctionCode.WriteSingleBit;

	/// <summary>
	///		数据地址。这里就是要写入的位的地址。
	/// </summary>
	public ushort DataAddress { get; set; } = 0;

	/// <summary>
	///		要写入的位的值
	/// </summary>
	public bool BitValue { get; set; } = false;

	/// <summary>
	///		序列化为字节数组
	/// </summary>
	/// <param name="big_endian"></param>
	/// <returns></returns>
	public byte[] ToBytes(bool big_endian)
	{
		AutoBitConverter auto_bit_converter = new(big_endian);
		byte[] frame = new byte[8];

		frame[0] = SlaveAddress;
		frame[1] = (byte)FunctionCode;

		byte[] data_addr_bytes = auto_bit_converter.GetBytes(DataAddress);
		frame[2] = data_addr_bytes[0];
		frame[3] = data_addr_bytes[1];

		byte[] data_bytes = auto_bit_converter.GetBytes(BitValue ? (ushort)0Xff00 : (ushort)0);
		frame[4] = data_bytes[0];
		frame[5] = data_bytes[1];

		ModbusCrc16 crc16 = new();
		crc16.Add(new Memory<byte>(frame, 0, 6));
		frame[6] = crc16.RegisterLowByte;
		frame[7] = crc16.RegisterHighByte;
		return frame;
	}
}
