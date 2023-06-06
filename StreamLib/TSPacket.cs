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
			Payload = new(reader, PayloadUnitStartIndicator);
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
		// 接下来还剩下 6 位保留位，于是就只剩一个最低位了
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
			LastPacketPayload = reader.ReadBytes(PayloadPointer);
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

	}

	public byte Table_ID { get; set; }
	public bool SectionSyntaxIndicator { get; set; }

	/*
		unsigned section_syntax_indicator     : 1; //段语法标志位，固定为1  
		unsigned zero                         : 1; //0  
		unsigned reserved_1                   : 2; // 保留位  
		 unsigned section_length               : 12; //表示从下一个字段开始到CRC32(含)之间有用的字节数  
		unsigned transport_stream_id          : 16; //该传输流的ID，区别于一个网络中其它多路复用的流  
		unsigned reserved_2                   : 2;// 保留位  
		unsigned version_number               : 5; //范围0-31，表示PAT的版本号  
		unsigned current_next_indicator       : 1; //发送的PAT是当前有效还是下一个PAT有效  
		unsigned section_number               : 8; //分段的号码。PAT可能分为多段传输，第一段为00，以后每个分段加1，最多可能有256个分段  
		unsigned last_section_number          : 8;  //最后一个分段的号码  

		std::vector<TS_PAT_Program> program;  
		unsigned reserved_3                    : 3; // 保留位  
		unsigned network_PID                    : 13; //网络信息表（NIT）的PID,节目号为0时对应的PID为network_PID  
		unsigned CRC_32                        : 32;  //CRC32校验码  
	 */
}