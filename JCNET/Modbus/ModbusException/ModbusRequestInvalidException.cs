namespace JCNET.Modbus.ModbusException;

public class ModbusRequestInvalidException : Exception
{
	public ModbusRequestInvalidException()
	{
	}

	public ModbusRequestInvalidException(string? message) : base(message)
	{
	}

	public ModbusRequestInvalidException(string? message, Exception? innerException) : base(message, innerException)
	{
	}
}
