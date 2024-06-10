namespace JCNET.Modbus.ModbusException;

public class ModbusException : Exception
{
	public ModbusException()
	{
	}

	public ModbusException(string? message) : base(message)
	{
	}

	public ModbusException(string? message, Exception? innerException) : base(message, innerException)
	{
	}
}
