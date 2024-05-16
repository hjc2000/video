namespace Modbus;

public class ModbusCrc16
{
	public ModbusCrc16() { }

	public ModbusCrc16(ushort polynomial)
	{
		_polynomial = polynomial;
	}

	/// <summary>
	///		默认的生成多项式是：
	///			x16+x15+x2+1
	/// 
	///		这个生成多项式转化成系数，从最低次的系数开始写，最低次是 x^0，系数是 1，
	///		写在最左边，然后 x^1 的系数是 0，接着写在右边，x^2 的系数是 1，接着写在
	///		右边，以此类推，得到：
	///			10100000000000011
	///		x^16 的系数是 1，但是最高次的系数是要被忽略的，因为最高次的系数一定是 1，
	///		例如这里的最高次是 16 次，如果 16 次的系数不是 1，那最高次就变成 15 次
	///		而不是 16 次了。
	/// 
	///		忽略最高次的系数后，得到
	///			1010000000000001
	///		这个数称为生成数。
	/// 
	///		这个生成数和生成多项式是反着来的，最高位对应的是生成多项式最低次的系数。
	///		转化为 16 进制就是 0xA001。
	/// </summary>
	private ushort _polynomial = 0xA001;

	public void Add(byte value)
	{
		Crc16RegisterValue ^= value;
		for (int i = 0; i < 8; i++)
		{
			ushort lsb = (ushort)(Crc16RegisterValue & 0x1);
			Crc16RegisterValue >>= 1;
			if (lsb != 0)
			{
				Crc16RegisterValue ^= _polynomial;
			}
		}
	}

	public void Add(IEnumerable<byte> datas)
	{
		foreach (byte value in datas)
		{
			Add(value);
		}
	}

	public ushort Crc16RegisterValue { get; private set; } = ushort.MaxValue;

	public byte Crc16RegisterValueHighByte
	{
		get
		{
			return (byte)(Crc16RegisterValueHighByte >> 8);
		}
	}

	public byte Crc16RegisterValueLowByte
	{
		get
		{
			return (byte)Crc16RegisterValue;
		}
	}
}
