namespace SDV3Lib.Modbus;

/// <summary>
///		功能码枚举。
/// </summary>
public enum FunctionCode
{
	/// <summary>
	///		读取一组线圈的状态。
	///		* 线圈表示输出位，只有 0 和 1 两种状态。
	///		* PLC 中的开关量输出控制的是继电器线圈，所以就有这个传统叫法。
	///		* 可以同时读取多个线圈。
	/// </summary>
	ReadCoils = 0x01,

	/// <summary>
	///		读取一组输入位的状态。
	///		* 相当于 PLC 中的开关量输入端子。
	///		* 可以同时读取多个输入位。
	/// </summary>
	ReadInputBits = 0x02,

	/// <summary>
	///		读取一组保持寄存器。
	///		* 一个保持寄存器就是一段 2 个字节的内存。
	///		* 可以一次性读取一组连续的保持寄存器。用这种方式可以一次性传输一段较大的内存。
	/// </summary>
	ReadHoldingRegisters = 0x03,

	/// <summary>
	///		读取一组输入寄存器。
	/// </summary>
	ReadInputRegisters = 0x04,

	/// <summary>
	///		写单个线圈。
	/// </summary>
	WriteSingleCoil = 0x05,

	/// <summary>
	///		写入多个线圈。
	/// </summary>
	WriteCoil = 0x0f,

	/// <summary>
	///		写入多个保持寄存器
	/// </summary>
	WriteHoldingRegisters = 0x10,

	/// <summary>
	///		诊断
	/// </summary>
	Diagnosis = 0x08,
}
