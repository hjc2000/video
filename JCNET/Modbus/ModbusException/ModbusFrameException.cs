namespace JCNET.Modbus.ModbusException;

/// <summary>
///		CRC 校验错误，响应帧中的功能码，数据地址等不符合预期，都会引发此异常。
/// </summary>
public class ModbusFrameException : Exception
{
	public ModbusFrameException()
	{
	}

	public ModbusFrameException(string? message) : base(message)
	{
	}

	public ModbusFrameException(string? message, Exception? innerException) : base(message, innerException)
	{
	}
}
