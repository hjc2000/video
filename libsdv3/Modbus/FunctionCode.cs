namespace libsdv3.Modbus;

public enum FunctionCode
{
	/// <summary>
	///		读取各种数据
	/// </summary>
	ReadDatas = 0x03,

	/// <summary>
	///		写入各种数据
	/// </summary>
	WriteDatas = 0x10,

	/// <summary>
	///		读取多个位
	/// </summary>
	ReadBits = 0x01,

	/// <summary>
	///		写入单个位
	/// </summary>
	WriteSingleBit = 0x05,

	/// <summary>
	///		写入多个位
	/// </summary>
	WriteBits = 0x0f,

	/// <summary>
	///		诊断
	/// </summary>
	Diagnosis = 0x08,
}
