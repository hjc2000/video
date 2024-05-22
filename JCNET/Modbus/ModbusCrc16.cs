namespace JCNET.Modbus;

public class ModbusCrc16
{
	public ModbusCrc16() { }

	public ModbusCrc16(ushort polynomial)
	{
		_polynomial = polynomial;
	}

	/// <summary>
	///		将 CRC16 寄存器重置为全 1，使本类对象恢复为刚构造时的样子。
	///		所有之前进行到一半的 CRC 计算全部终止，计算结果作废。
	/// </summary>
	public void Reset()
	{
		Register = ushort.MaxValue;
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
		Register ^= value;
		for (int i = 0; i < 8; i++)
		{
			ushort lsb = (ushort)(Register & 0x1);
			Register >>= 1;
			if (lsb != 0)
			{
				Register ^= _polynomial;
			}
		}
	}

	public void Add(Memory<byte> datas)
	{
		foreach (byte value in datas.Span)
		{
			Add(value);
		}
	}

	public ushort Register { get; private set; } = ushort.MaxValue;

	public byte RegisterHighByte
	{
		get
		{
			return (byte)(Register >> 8);
		}
	}

	public byte RegisterLowByte
	{
		get
		{
			return (byte)Register;
		}
	}
}
