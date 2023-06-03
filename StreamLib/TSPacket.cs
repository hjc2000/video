using System.Text;

namespace StreamLib;
public class TSPacket
{
	public TSPacket(byte[] tsPacket)
	{
		MemoryStream tsPacketBuffStream = new(tsPacket);
		BinaryReader reader = new(tsPacketBuffStream);
		// 读取同步字节
		SyncByte = reader.ReadByte();
		// 读取两个字节
		ushort temp = reader.ReadUInt16();
		Console.WriteLine(temp);
		TransportErrorIndicator = BitView.ReadBit(temp, 15) != 0;
		PayloadUnitStartIndicator = BitView.ReadBit(temp, 14) != 0;
		TransportPriority = BitView.ReadBit(temp, 13) != 0;
		PID = (ushort)BitView.ReadBits(temp, 0, 13);
		// 读取一个字节
		byte temp1 = reader.ReadByte();
		// 读取第6、7位
		TransportScramblingControl = (byte)BitView.ReadBits(temp1, 6, 2);
		// 读取第4、5位
		AdaptationFieldControl = (byte)BitView.ReadBits(temp1, 4, 2);
		ContinuityCounter = (byte)BitView.ReadBits(temp1, 0, 4);
	}

	public byte SyncByte { get; set; }
	public bool TransportErrorIndicator { get; set; } = false;
	public bool PayloadUnitStartIndicator { get; set; } = false;
	public bool TransportPriority { get; set; } = true;
	public ushort PID { get; set; }
	public byte TransportScramblingControl { get; set; }
	public byte AdaptationFieldControl { get; set; }
	public byte ContinuityCounter { get; set; }

	public override string ToString()
	{
		StringBuilder sb = new();
		sb.AppendLine($"sync_byte=0x{SyncByte:x}");
		sb.AppendLine($"transport_error_indicator={TransportErrorIndicator}");
		sb.AppendLine($"payload_unit_start_indicator={PayloadUnitStartIndicator}");
		sb.AppendLine($"transport_priority={TransportPriority}");
		sb.AppendLine($"PID={PID}");
		sb.AppendLine($"transport_scrambling_control={TransportScramblingControl}");
		sb.AppendLine($"adaptation_field_control={AdaptationFieldControl}");
		sb.AppendLine($"continuity_counter={ContinuityCounter}");
		return sb.ToString();
	}
}
