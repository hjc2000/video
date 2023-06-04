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
		temp = reader.ReadByte();
		TransportScramblingControl = (byte)BitView.ReadBits(temp, 6, 7);
		AdaptationFieldControl = (byte)BitView.ReadBits(temp, 4, 5);
		ContinuityCounter = (byte)BitView.ReadBits(temp, 0, 3);
		// 调整域
		if (AdaptationFieldControl == 2 || AdaptationFieldControl == 3)
		{
			// 存在调整域
			AdaptationField = new(reader);
		}
		// 数据域
		if (AdaptationFieldControl == 1 || AdaptationFieldControl == 3)
		{
			// 存在数据域
		}
	}

	public byte SyncByte { get; set; }
	public bool TransportErrorIndicator { get; set; } = false;
	public bool PayloadUnitStartIndicator { get; set; } = false;
	public bool TransportPriority { get; set; } = true;
	public ushort PID { get; set; }
	public byte TransportScramblingControl { get; set; }
	public byte AdaptationFieldControl { get; set; }
	public byte ContinuityCounter { get; set; }
	public AdaptationField? AdaptationField { get; set; } = null;
	public byte[]? Data { get; set; } = null;

	public override string ToString()
	{
		StringBuilder sb = new();
		sb.AppendLine($"sync_byte=0x{SyncByte:x}");
		sb.AppendLine($"transport_error_indicator={TransportErrorIndicator}");
		sb.AppendLine($"payload_unit_start_indicator={PayloadUnitStartIndicator}");
		sb.AppendLine($"transport_priority={TransportPriority}");
		sb.AppendLine($"PID=0x{PID:x4}");
		sb.AppendLine($"transport_scrambling_control={TransportScramblingControl}");
		sb.AppendLine($"adaptation_field_control={AdaptationFieldControl}");
		sb.AppendLine($"continuity_counter={ContinuityCounter}");
		if (AdaptationField != null)
		{
			sb.AppendLine(AdaptationField.ToString());
		}

		return sb.ToString();
	}
}

public class AdaptationField
{
	public AdaptationField(BinaryReader reader)
	{
		AdaptationFieldLength = reader.ReadByte();
		byte temp = reader.ReadByte();
		int bitIndex = 7;
		DiscontinuityIndicator = BitView.ReadBit(temp, bitIndex--);
		RandomAccessIndicator = BitView.ReadBit(temp, bitIndex--);
		ElementaryStreamPriorityIndicator = BitView.ReadBit(temp, bitIndex--);
		PCR_Flag = BitView.ReadBit(temp, bitIndex--);
		OPCR_Flag = BitView.ReadBit(temp, bitIndex--);
		SplicingPointFlag = BitView.ReadBit(temp, bitIndex--);
		TransportPrivateDataFlag = BitView.ReadBit(temp, bitIndex--);
		AdaptationFieldExtensionFlag = BitView.ReadBit(temp, bitIndex--);
		if (PCR_Flag)
		{
			CalculatePCR(reader);
		}
	}

	public byte AdaptationFieldLength { get; set; }
	public bool DiscontinuityIndicator { get; set; }
	public bool RandomAccessIndicator { get; set; }
	public bool ElementaryStreamPriorityIndicator { get; set; }
	public bool PCR_Flag { get; set; }
	public bool OPCR_Flag { get; set; }
	public bool SplicingPointFlag { get; set; }
	public bool TransportPrivateDataFlag { get; set; }
	public bool AdaptationFieldExtensionFlag { get; set; }
	public ulong PCR { get; set; }

	public override string ToString()
	{
		StringBuilder sb = new();
		sb.AppendLine($"\t adaptation_field_length={AdaptationFieldLength}");
		sb.AppendLine($"\t discontinuity_indicator={DiscontinuityIndicator}");
		sb.AppendLine($"\t RandomAccessIndicator={RandomAccessIndicator}");
		sb.AppendLine($"\t ElementaryStreamPriorityIndicator={ElementaryStreamPriorityIndicator}");
		sb.AppendLine($"\t PCR_Flag={PCR_Flag}");
		sb.AppendLine($"\t OPCR_Flag={OPCR_Flag}");
		sb.AppendLine($"\t SplicingPointFlag={SplicingPointFlag}");
		sb.AppendLine($"\t TransportPrivateDataFlag={TransportPrivateDataFlag}");
		sb.AppendLine($"\t AdaptationFieldExtensionFlag={AdaptationFieldExtensionFlag}");
		if (PCR_Flag)
		{
			sb.AppendLine($"\t PCR={PCR}");
		}

		return sb.ToString();
	}

	private void CalculatePCR(BinaryReader reader)
	{
		ulong pcrBase = reader.ReadUInt32();
		// PCR 的 base 是 33 位，所以再左移一位
		pcrBase <<= 1;
		// PCR 总共有 6 个字节，已经读取了 4 个字节，还剩 2 个字节
		byte high = reader.ReadByte();
		// 这个字节中的最高位是属于 base 的
		if (BitView.ReadBit(high, 7))
		{
			pcrBase += 1;
		}
		// 接下来还剩下 6 位保留位，于是就只剩一个最低位了
		high &= 1;
		byte low = reader.ReadByte();
		ushort extension = (ushort)((high << 8) | low);
		PCR = (pcrBase * 300) + extension;
	}
}