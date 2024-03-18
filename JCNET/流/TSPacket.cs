using System.Text.Json;
using System.Text.Json.Serialization;

namespace JCNET.流;

public class TSPacket
{
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="tsPacket">
	/// 188 字节的一个 ts 包去掉同步字节，剩下 187 字节，传递给本构造函数。解封装器
	/// 应该不断读取字节流，知道读取到同步字节，然后读取后面的 187 个字节传递给本构造函数
	/// </param>
	public TSPacket(byte[] tsPacket)
	{
		using MemoryStream tsPacketBuffStream = new(tsPacket);
		using BinaryReader reader = new(tsPacketBuffStream);
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
		// 自适应场
		if (AdaptationFieldControl is 2 or 3)
		{
			AdaptationField = new(reader);
		}
		// 负载
		if (AdaptationFieldControl is 1 or 3)
		{
			Payload = new(reader, PayloadUnitStartIndicator);
		}
	}

	#region TS 包字段
	/// <summary>
	/// 传输错误标志。出现错误时为 true
	/// </summary>
	public bool TransportErrorIndicator { get; set; } = false;
	public bool PayloadUnitStartIndicator { get; set; } = false;
	/// <summary>
	/// 传输优先级标志。如果此包比同 PID 的其他包具有更高的优先级则为 true
	/// </summary>
	public bool TransportPriority { get; set; } = true;
	/// <summary>
	/// 13 位的整型
	/// </summary>
	public ushort PID { get; set; }
	/// <summary>
	/// 传输加扰指示。00 表示未加扰，其他表示加扰
	/// </summary>
	public byte TransportScramblingControl { get; set; }
	/// <summary>
	/// 自适应场控制				<br></br>
	/// 00 - 保留				<br></br>
	/// 01 - 无自适应场，有负载	<br></br>
	/// 10 - 有自适应场，无负载	<br></br>
	/// 11 - 有自适应场，有负载	<br></br>
	/// * 可以看出，左边一位表示有无自适应场，右边一位表示有无负载。		<br></br>
	/// * 空包的自适应场控制应设置为 01
	/// </summary>
	public byte AdaptationFieldControl { get; set; }
	/// <summary>
	/// 连续性计数器													<br></br>
	/// * 只有含有负载时才能递增此值，即自适应场控制应为 01 或 11			<br></br>
	/// * 数据包可以出现重复，重复的数据包不得递增此值						<br></br>
	/// * 空数据包的连续性计数器值未定义									<br></br>
	/// </summary>
	public byte ContinuityCounter { get; set; }
	/// <summary>
	/// 自适应场
	/// </summary>
	public AdaptationField? AdaptationField { get; set; } = null;
	/// <summary>
	/// 负载
	/// </summary>
	public Payload? Payload { get; set; } = null;
	#endregion

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

/// <summary>
/// 自适应场
/// </summary>
public class AdaptationField
{
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="reader">二进制阅读器</param>
	public AdaptationField(BinaryReader reader)
	{
		// 第一个字节指出自适应场的长度
		AdaptationFieldLength = reader.ReadByte();
		// 自适应场的长度可能为 0，当长度为 0 时，自适应场的作用是填充单个字节（就是插入无用数据）
		// 此外，自适应场的长度可以超出自适应场中有效数据的长度之和，即自适应场后面会跟着一些无用数据。
		// 通过这种方式可以填充 ts 包。当负载的长度不够，不足以使 ts 包长度到达 188 字节时就可以让自适应
		// 场用这种方式来填充，使 ts 包达到 188 字节。

		// 对于携带 PES 数据的 ts 包，这是唯一的填充方式。对于携带 PSI 信息的 ts 包，还有另外一种填充方式
		if (AdaptationFieldLength > 0)
		{
			// 把自适应场全部读入缓冲区
			byte[] adaptationFieldBuff = reader.ReadBytes(AdaptationFieldLength);
			using MemoryStream memoryStream = new(adaptationFieldBuff);
			// 为了防止自适应场长度不符合标准，导致对 memoryStream 的读取发生“流到达尽头”异常
			// 下面使用 try 块包住
			try
			{
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

					/// <summary>
					/// 计算 PCR 或 OPCR
					/// </summary>
					/// <param name="reader"></param>
					/// <returns></returns>
					static ulong CalculatePCR(BinaryReader reader)
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

					// 如果自适应场中携带 PCR，取出 PCR
					if (PCR_Flag)
					{
						PCR = CalculatePCR(reader);
					}
					// 如果自适应场中携带 OPCR，取出 OPCR
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
						if (TransportPrivateDataLength > 0)
						{
							TransportPrivateData = reader.ReadBytes(TransportPrivateDataLength);
						}
					}

					if (AdaptationFieldExtensionFlag)
					{
						// 自适应场的扩展区
						AdaptationExtension = new(reader);
					}
				}
			}
			catch { }
		}
	}

	public byte AdaptationFieldLength { get; set; }

	#region 标志字段
	/// <summary>
	/// 不连续标志。
	/// * 可能指 PCR 不连续，也可能指连续计数器不连续。如果当前包携带 PCR 且与之前的 PCR 不连续，则应
	/// 将这个标志设置为 true.
	/// 
	/// * PCR 发生一次间断后，接下来两次收到的 PCR 必须是连续的，	不允许立刻再发生一次 PCR 间断。
	/// </summary>
	public bool DiscontinuityIndicator { get; set; }
	public bool RandomAccessIndicator { get; set; }
	public bool ElementaryStreamPriorityIndicator { get; set; }
	public bool PCR_Flag { get; set; }
	public bool OPCR_Flag { get; set; }
	public bool SplicingPointFlag { get; set; }
	public bool TransportPrivateDataFlag { get; set; }
	public bool AdaptationFieldExtensionFlag { get; set; }
	#endregion

	#region 可选字段
	public ulong PCR { get; set; }
	public ulong OPCR { get; set; }
	public sbyte SpliceCountdown { get; set; }
	public byte TransportPrivateDataLength { get; set; }
	public byte[]? TransportPrivateData { get; set; }
	public AdaptationExtension? AdaptationExtension { get; set; }
	#endregion
}

public class AdaptationExtension
{
	public AdaptationExtension(BinaryReader reader)
	{
		AdaptationFieldExtensionLength = reader.ReadByte();
		if (AdaptationFieldExtensionLength > 0)
		{
			// 创建新的阅读器
			byte[] buffer = reader.ReadBytes(AdaptationFieldExtensionLength);
			using MemoryStream mstream = new(buffer);
			reader = new BinaryReader(mstream);
			// 使用新的阅读器
			BitStream_lbf bslbf = new(reader.ReadByte());
			LtwFlag = bslbf.ReadBit();
			PiecewiseRateFlag = bslbf.ReadBit();
			SeamlessSpliceFlag = bslbf.ReadBit();
			if (LtwFlag)
			{
				bslbf = new(reader.ReadByte());
				LtwValidFlag = bslbf.ReadBit();
				byte high = bslbf.ReadBits(7);
				byte low = reader.ReadByte();
				LtwOffset = SpliceByte.SpliceIntoUint16(high, low);
			}

			if (PiecewiseRateFlag)
			{
				byte[] buffer1 = reader.ReadBytes(3);
				buffer1[0] = BitView.ReadBits(buffer1[0], 0, 5);
				PiecewiseRate = SpliceByte.SpliceIntoUint32(buffer1);
			}

			if (SeamlessSpliceFlag)
			{
				bslbf = new(reader.ReadByte());
				SpliceType = bslbf.ReadBits(4);
			}
		}
	}

	#region 必需字段
	public byte AdaptationFieldExtensionLength { get; set; }
	public bool LtwFlag { get; set; } = false;
	public bool PiecewiseRateFlag { get; set; } = false;
	public bool SeamlessSpliceFlag { get; set; } = false;
	#endregion

	#region 可选字段
	public bool LtwValidFlag { get; set; } = false;
	public ushort LtwOffset { get; set; }
	public uint PiecewiseRate { get; set; }
	public byte SpliceType { get; set; }
	#endregion
}

/// <summary>
/// 负载
/// </summary>
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
	public List<PAT_Program> PAT_ProgrameList { get; set; } = [];
	public uint CRC32 { get; set; }
	public string CRC32_Str
	{
		get
		{
			return $"{CRC32:x}";
		}
	}

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