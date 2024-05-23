namespace JCNET.Modbus.ModbusException;

/// <summary>
///		请求的数据的地址错误，从机返回了 0x2 例外码。
/// </summary>
public class ModbusRequestDataAddressInvalidException : ModbusRequestInvalidException
{
	public ModbusRequestDataAddressInvalidException()
	{
	}

	public ModbusRequestDataAddressInvalidException(string? message) : base(message)
	{
	}

	public ModbusRequestDataAddressInvalidException(string? message, Exception? innerException) : base(message, innerException)
	{
	}
}
