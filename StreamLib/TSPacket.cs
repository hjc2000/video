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
		byte temp = reader.ReadByte();
		TransportErrorIndicator = BitView.ReadBit(temp, 7);
		PayloadUnitStartIndicator = BitView.ReadBit(temp, 6);
		TransportPriority = BitView.ReadBit(temp, 5);
		// 剩下的 5 位和下一个字节拼在一起组成 PID
		temp = (byte)(temp & 0b_000_11111);
		byte low = reader.ReadByte();
		PID = (ushort)((temp << 8) | low);
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
		sb.AppendLine($"PID=0x{PID:x}");
		sb.AppendLine($"transport_scrambling_control={TransportScramblingControl}");
		sb.AppendLine($"adaptation_field_control={AdaptationFieldControl}");
		sb.AppendLine($"continuity_counter={ContinuityCounter}");
		return sb.ToString();
	}
}
