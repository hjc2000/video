namespace JCNET.Modbus.ModbusException;

/// <summary>
///		响应帧中的 CRC 校验错误，功能码，数据地址等不符合预期，都会引发此异常。
/// </summary>
public class ModbusResponseException : ModbusException
{
	public ModbusResponseException()
	{
	}

	public ModbusResponseException(string? message) : base(message)
	{
	}

	public ModbusResponseException(string? message, Exception? innerException) : base(message, innerException)
	{
	}
}
