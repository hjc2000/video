namespace JCNET.Modbus.ModbusException;

/// <summary>
///		请求的功能码是非法的，冲击返回了 0x1 例外码。
/// </summary>
public class ModbusRequestFunctionCodeInvalidException : ModbusRequestInvalidException
{
	public ModbusRequestFunctionCodeInvalidException()
	{
	}

	public ModbusRequestFunctionCodeInvalidException(string? message) : base(message)
	{
	}

	public ModbusRequestFunctionCodeInvalidException(string? message, Exception? innerException) : base(message, innerException)
	{
	}
}
