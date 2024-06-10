namespace JCNET.Modbus.ModbusException;

public class ModbusRequestException : Exception
{
	public ModbusRequestException()
	{
	}

	public ModbusRequestException(string? message) : base(message)
	{
	}

	public ModbusRequestException(string? message, Exception? innerException) : base(message, innerException)
	{
	}
}
