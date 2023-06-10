using System.Text.Json;
using System.Text.Json.Serialization;

namespace StreamLib;

public class TSPacket
{
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="tsPacket">188 字节的一个 ts 包</param>
	public TSPacket(byte[] tsPacket)
	{
		using MemoryStream tsPacketBuffStream = new(tsPacket);
		using BinaryReader reader = new(tsPacketBuffStream);
		// 读取同步字节
		SyncByte = reader.ReadByte();
		byte temp = reader.ReadByte();
		BitStream_lbf bslbf = new(temp);
		TransportErrorIndicator = bslbf.ReadBit();
		PayloadUnitStartIndicator = bslbf.ReadBit();
		TransportPriority = bslbf.ReadBit();
		// 剩下的 5 位和下一个字节拼在一起组成 PID
		temp = (byte)(temp & 0b_000_11111);
		byte low = reader.ReadByte();
		PID = (ushort)((temp << 8) | low);
		temp = reader.ReadByte();
		bslbf = new BitStream_lbf(temp);
		TransportScramblingControl = bslbf.ReadBits(2);
		AdaptationFieldControl = bslbf.ReadBits(2);
		ContinuityCounter = bslbf.ReadBits(4);
		// 自适应域
		if (AdaptationFieldControl == 2 || AdaptationFieldControl == 3)
		{
			AdaptationField = new(reader);
		}
		// 数据域
		if (AdaptationFieldControl == 1 || AdaptationFieldControl == 3)
		{
			Payload = new(reader, PayloadUnitStartIndicator);
		}
	}

	public byte SyncByte { get; set; }
	public bool TransportErrorIndicator { get; set; } = false;
	public bool PayloadUnitStartIndicator { get; set; } = false;
	public bool TransportPriority { get; set; } = true;
	public ushort PID { get; set; }
	/// <summary>
	/// 传输加扰指示。00 表示未加扰，其他表示加扰
	/// </summary>
	public byte TransportScramblingControl { get; set; }
	/// <summary>
	/// 自适应域指示。				<br></br>
	/// 00 - 保留				<br></br>
	/// 01 - 无自适应域，有负载	<br></br>
	/// 10 - 有自适应域，无负载	<br></br>
	/// 11 - 有自适应域，有负载	<br></br>
	/// * 可以看出，左边一位表示有无自适应域，右边一位表示有无负载。		<br></br>
	/// * 空包的自适应字段应设置为 01
	/// </summary>
	public byte AdaptationFieldControl { get; set; }
	/// <summary>
	/// 连续性计数器												<br></br>
	/// * 只有含有负载时才能递增此值，即自适应域指示应为 01 或 11		<br></br>
	/// * 数据包可以出现重复，重复的数据包不得递增此值					<br></br>
	/// * 空数据包的连续性计数器值未定义								<br></br>
	/// </summary>
	public byte ContinuityCounter { get; set; }
	/// <summary>
	/// 自适应域
	/// </summary>
	public AdaptationField? AdaptationField { get; set; } = null;
	/// <summary>
	/// 负载
	/// </summary>
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
		// 每读取一个字节就递增
		AdaptationFieldLength = reader.ReadByte();
		// 把自适应区全部读入缓冲区
		byte[] adaptationFieldBuff = reader.ReadBytes(AdaptationFieldLength);
		using MemoryStream memoryStream = new(adaptationFieldBuff);
		using (reader = new BinaryReader(memoryStream))
		{
			byte temp = reader.ReadByte();
			BitStream_lbf bslbf = new(temp);
			DiscontinuityIndicator = bslbf.ReadBit();
			RandomAccessIndicator = bslbf.ReadBit();
			ElementaryStreamPriorityIndicator = bslbf.ReadBit();
			PCR_Flag = bslbf.ReadBit();
			OPCR_Flag = bslbf.ReadBit();
			SplicingPointFlag = bslbf.ReadBit();
			TransportPrivateDataFlag = bslbf.ReadBit();
			AdaptationFieldExtensionFlag = bslbf.ReadBit();
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
				// 私有数据
				TransportPrivateDataLength = reader.ReadByte();
				TransportPrivateData = reader.ReadBytes(TransportPrivateDataLength);
			}

			if (AdaptationFieldExtensionFlag)
			{
				// 自适应区的扩展区
				AdaptationExtension = new(reader);
			}
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
	public AdaptationExtension? AdaptationExtension { get; set; }
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
		// 接下来的 6 位是保留位，只剩一个最低位是有用的，把它取出来
		high &= 1;
		byte low = reader.ReadByte();
		ushort extension = (ushort)((high << 8) | low);
		return (pcrBase * 300) + extension;
	}
}

public class AdaptationExtension
{
	public AdaptationExtension(BinaryReader reader)
	{

	}
}

public class Payload
{
	public Payload(BinaryReader reader, bool payloadUnitStartIndicator)
	{
		if (payloadUnitStartIndicator)
		{
			// payloadUnitStartIndicator=true 表示有上一个数据包的负载搭了这个数据包的便车，
			// 需要将上一个数据包的负载分离出来
			PayloadPointer = reader.ReadByte();
			if (PayloadPointer > 0)
			{
				LastPacketPayload = reader.ReadBytes(PayloadPointer);
			}
		}
		// 读完剩下的所有内容
		ActualPayload = reader.ReadBytes((int)(reader.BaseStream.Length - reader.BaseStream.Position));
	}

	/// <summary>
	/// 指示从第几个字节开始（索引号从0开始）是这个数据包的负载，在这之前的都是上一个数据包的负载
	/// </summary>
	public byte PayloadPointer { get; set; }
	/// <summary>
	/// 上一个数据包的负载
	/// </summary>
	public byte[]? LastPacketPayload { get; set; }
	public byte[] ActualPayload { get; set; }
}

/// <summary>
/// https://www.cnblogs.com/shakin/p/3714848.html
/// </summary>
public class PAT
{
	public PAT(byte[] dataBuffer)
	{
		using MemoryStream memoryStream = new(dataBuffer);
		using BinaryReader reader = new(memoryStream);
		Table_ID = reader.ReadByte();
		if (Table_ID != 0)
		{
			Console.WriteLine("这不是PAT表");
			throw new Exception("这不是PAT表");
		}

		byte temp = reader.ReadByte();
		BitStream_lbf bslbf = new(temp);
		SectionSyntaxIndicator = bslbf.ReadBit();
		Zero = bslbf.ReadBit();
		Reserved1 = bslbf.ReadBits(2);
		byte high = bslbf.ReadBits(4);
		byte low = reader.ReadByte();
		SectionLength = (ushort)((high << 8) | low);
		high = reader.ReadByte();
		low = reader.ReadByte();
		TransportStreamID = (ushort)((high << 8) | low);
		temp = reader.ReadByte();
		bslbf = new BitStream_lbf(temp);
		Reserved2 = bslbf.ReadBits(2);
		VersionNumber = bslbf.ReadBits(5);
		CurrentNextIndicator = bslbf.ReadBit();
		SectionNumber = reader.ReadByte();
		LastSectionNumber = reader.ReadByte();
		// 流的 Position 指示的是下一次读取时的索引号
		// SectionLength 指示储存节目信息的区域加上末尾的 CRC32 的总长度
		// 到最后的 4 字节之前都是节目信息，所以在循环内解析节目信息，直到剩下 4 个字节
		while (SectionLength - reader.BaseStream.Position > 4)
		{
			high = reader.ReadByte();
			low = reader.ReadByte();
			ushort programNumber = SpliceByte.SpliceIntoUint16(high, low);
			high = reader.ReadByte();
			high = (byte)(high & 0b_000_11111);
			low = reader.ReadByte();
			ushort pid = SpliceByte.SpliceIntoUint16(high, low);
			PAT_ProgrameList.Add(new PAT_Program()
			{
				ProgramNumber = programNumber,
				PMT_PID = pid,
			});
			if (reader.BaseStream.Position == 180)
			{
				Console.WriteLine();
			}
		}
		// 解析出 CRC32
		CRC32 = SpliceByte.SpliceIntoUint32(reader.ReadBytes(4));
	}

	public byte Table_ID { get; set; }
	public bool SectionSyntaxIndicator { get; set; }
	public bool Zero { get; set; }
	public byte Reserved1 { get; set; }
	public ushort SectionLength { get; set; }
	public ushort TransportStreamID { get; set; }
	public byte Reserved2 { get; set; }
	public byte VersionNumber { get; set; }
	public bool CurrentNextIndicator { get; set; }
	public byte SectionNumber { get; set; }
	public byte LastSectionNumber { get; set; }
	public List<PAT_Program> PAT_ProgrameList { get; set; } = new List<PAT_Program>();
	public uint CRC32 { get; set; }
	public string CRC32_Str => $"{CRC32:x}";

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

public class PAT_Program
{
	public ushort ProgramNumber { get; set; }
	public ushort PMT_PID { get; set; }
}