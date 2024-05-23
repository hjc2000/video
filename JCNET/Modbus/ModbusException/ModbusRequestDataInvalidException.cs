namespace JCNET.Modbus.ModbusException;

/// <summary>
///		请求的数据有错误，从机返回了 0x3 例外码。
/// </summary>
public class ModbusRequestDataInvalidException : ModbusRequestInvalidException
{
	public ModbusRequestDataInvalidException()
	{
	}

	public ModbusRequestDataInvalidException(string? message) : base(message)
	{
	}

	public ModbusRequestDataInvalidException(string? message, Exception? innerException) : base(message, innerException)
	{
	}
}
