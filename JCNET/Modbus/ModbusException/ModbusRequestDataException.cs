namespace JCNET.Modbus.ModbusException;

/// <summary>
///		请求的数据有错误，从机返回了 0x3 例外码。
/// </summary>
public class ModbusRequestDataException : ModbusRequestException
{
	public ModbusRequestDataException()
	{
	}

	public ModbusRequestDataException(string? message) : base(message)
	{
	}

	public ModbusRequestDataException(string? message, Exception? innerException) : base(message, innerException)
	{
	}
}
