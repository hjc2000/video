namespace libsdv3.Code;

public enum AlarmCode
{
	/// <summary>
	///		无错误
	/// </summary>
	None = 0x00,

	/// <summary>
	///		过电流 1
	/// </summary>
	OverCurrent1 = 0x01,

	/// <summary>
	///		过电流 2
	/// </summary>
	OverCurrent2 = 0x02,

	/// <summary>
	///		超速
	/// </summary>
	OverSpeed = 0x03,

	/// <summary>
	///		控制电路电压不足
	/// </summary>
	CcontrolCircuitLowVoltage = 0x04,

	/// <summary>
	///		过电压
	/// </summary>
	OverVoltage = 0x05,

	/// <summary>
	///		编码器异常 1
	/// </summary>
	EncoderException1 = 0x06,

	/// <summary>
	///		编码器异常 2
	/// </summary>
	EncoderException2 = 0x07,

	/// <summary>
	///		控制电路异常
	/// </summary>
	CcontrolCircuitException = 0x08,

	/// <summary>
	///		储存器异常
	/// </summary>
	StorageDeviceException = 0x09,

	/// <summary>
	///		电机组合异常
	/// </summary>
	ElectricalMachineryCombinationException = 0x0B,

	/// <summary>
	///		编码器通信异常
	/// </summary>
	EncoderCommunicationException = 0x0D,

	/// <summary>
	///		EI 重复
	/// </summary>
	EIDuplication = 0x0E,

	/// <summary>
	///		过载 1
	/// </summary>
	OverLoad1 = 0x0F,

	/// <summary>
	///		过载 1
	/// </summary>
	OverLoad2 = 0x10,

	/// <summary>
	///		过电流 3
	/// </summary>
	OverCurrent3 = 0x12,

	/// <summary>
	///		编码器匹配异常
	/// </summary>
	EncoderMatchingException = 0x13,

	/// <summary>
	///		外部端子报警
	/// </summary>
	ExternalTerminalAlarm = 0x20,

	/// <summary>
	///		主电路电压不足
	/// </summary>
	MainCircuitLowVoltage = 0x21,

	/// <summary>
	///		回生晶体管异常
	/// </summary>
	RegenerativeTransistorException = 0x24,

	/// <summary>
	///		偏差超出
	/// </summary>
	DeviationOverflow = 0x25,

	/// <summary>
	///		驱动器过热
	/// </summary>
	DriverTooHot = 0x26,

	/// <summary>
	///		ABS数据丢失 1
	/// </summary>
	AbsDataLoss1 = 0x28,

	/// <summary>
	///		ABS数据丢失 2
	/// </summary>
	AbsDataLoss2 = 0x29,

	/// <summary>
	///		ABS数据丢失 3
	/// </summary>
	AbsDataLoss3 = 0x2A,

	/// <summary>
	///		多圈溢出
	/// </summary>
	MultiLoopOverflow = 0x2B,

	/// <summary>
	///		原点复归超时
	/// </summary>
	OriginRestorationTimeout = 0x2E,

	/// <summary>
	///		失速
	/// </summary>
	Stall = 0x2F,
}
