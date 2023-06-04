using System.Text.Json;
using System.Text.Json.Serialization;

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
			Payload = new(reader);
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
	public Payload? Payload { get; set; } = null;

	public override string ToString()
	{
		using MemoryStream memoryStream = new();
		JsonSerializer.Serialize(memoryStream, this, new JsonSerializerOptions()
		{
			AllowTrailingCommas = true,
			WriteIndented = true,
			NumberHandling = JsonNumberHandling.Strict,
			DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
		});
		memoryStream.Position = 0;
		StreamReader jsonStreamReader = new(memoryStream);
		return jsonStreamReader.ReadToEnd();
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
			PCR = CalculatePCR(reader);
		}

		if (OPCR_Flag)
		{
			OPCR = CalculatePCR(reader);
		}

		if (SplicingPointFlag)
		{
			SpliceCountdown = reader.ReadSByte();
		}

		if (TransportPrivateDataFlag)
		{
			TransportPrivateDataLength = reader.ReadByte();
			TransportPrivateData = reader.ReadBytes(TransportPrivateDataLength);
		}
	}

	public byte AdaptationFieldLength { get; set; }

	#region 标志域
	public bool DiscontinuityIndicator { get; set; }
	public bool RandomAccessIndicator { get; set; }
	public bool ElementaryStreamPriorityIndicator { get; set; }
	public bool PCR_Flag { get; set; }
	public bool OPCR_Flag { get; set; }
	public bool SplicingPointFlag { get; set; }
	public bool TransportPrivateDataFlag { get; set; }
	public bool AdaptationFieldExtensionFlag { get; set; }
	#endregion

	#region 可选域
	public ulong PCR { get; set; }
	public ulong OPCR { get; set; }
	public sbyte SpliceCountdown { get; set; }
	public byte TransportPrivateDataLength { get; set; }
	public byte[]? TransportPrivateData { get; set; }
	#endregion

	/// <summary>
	/// 计算 PCR 或 OPCR
	/// </summary>
	/// <param name="reader"></param>
	/// <returns></returns>
	private static ulong CalculatePCR(BinaryReader reader)
	{
		ulong pcrBase = 0;
		for (int i = 0; i < 4; i++)
		{
			pcrBase <<= 8;
			pcrBase += reader.ReadByte();
		}
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
		return (pcrBase * 300) + extension;
	}
}

public class Payload
{
	public Payload(BinaryReader reader)
	{
		PayloadPointer = reader.ReadByte();
		ActualPayload = reader.ReadBytes((int)(reader.BaseStream.Length - reader.BaseStream.Position));
	}

	public byte PayloadPointer { get; set; }
	public byte[] ActualPayload { get; set; }
}