namespace libsdv3.Modbus;

public enum FunctionCode
{
	ReadBits = 0x01,
	WriteSingleBit = 0x05,
}

public interface IModbusDevice
{
	void WriteSingleBit(ushort addr, bool value);
}
