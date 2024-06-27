using JCNET;
using JCNET.Modbus;

namespace SDV3Lib.Modbus.Frame;

public class WriteDatasRequestFrame
{
	public byte SlaveAddress { get; set; } = 1;
	private FunctionCode FunctionCode { get; } = FunctionCode.WriteHoldingRegisters;
	public ushort DataAddress { get; set; } = 0;

	private ushort RecordCount
	{
		get
		{
			return (byte)(Datas.Length * 2);
		}
	}

	private byte ByteCount
	{
		get
		{
			return (byte)(Datas.Length * 4);
		}
	}

	public uint[] Datas { get; set; } = [];

	public byte[] ToBytes(bool big_endian)
	{
		AutoBitConverter auto_bit_converter = new(big_endian);
		byte[] frame = new byte[9 + ByteCount];

		frame[0] = SlaveAddress;
		frame[1] = (byte)FunctionCode;

		byte[] data_addr_bytes = auto_bit_converter.GetBytes(DataAddress);
		frame[2] = data_addr_bytes[0];
		frame[3] = data_addr_bytes[1];

		byte[] record_count_bytes = auto_bit_converter.GetBytes(RecordCount);
		frame[4] = record_count_bytes[0];
		frame[5] = record_count_bytes[1];

		frame[6] = ByteCount;

		for (int i = 0; i < Datas.Length; i++)
		{
			byte[] datas_bytes = auto_bit_converter.GetBytes(Datas[i]);
			frame[7 + (i * 4) + 0] = datas_bytes[0];
			frame[7 + (i * 4) + 1] = datas_bytes[1];
			frame[7 + (i * 4) + 2] = datas_bytes[2];
			frame[7 + (i * 4) + 3] = datas_bytes[3];
		}

		ModbusCrc16 crc16 = new();
		crc16.Add(new Memory<byte>(frame, 0, frame.Length - 2));
		frame[^2] = crc16.RegisterLowByte;
		frame[^1] = crc16.RegisterHighByte;
		return frame;
	}
}
