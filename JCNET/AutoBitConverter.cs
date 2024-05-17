namespace JCNET;

/// <summary>
///		能够自动根据本机字节序和目标字节序来决定是否翻转字节数组后再转化。
/// </summary>
public class AutoBitConverter
{
	/// <summary>
	///		
	/// </summary>
	/// <param name="big_endian">
	///		如果为 true，则：
	///			* 将字节数组转化为整型时，将字节数组视为大端序。
	///			* 将整型转化为字节数组时，输出的字节数组是大端序的。
	///		如果为 false，则：
	///			* 将字节数组转化为整型时，将字节数组视为小端序。
	///			* 将整型转化为字节数组时，输出的字节数组是小端序的。
	/// </param>
	public AutoBitConverter(bool big_endian)
	{
		IsBigEndian = big_endian;
	}

	private bool IsBigEndian { get; set; } = false;
	private bool IsLittleEndian
	{
		get
		{
			return !IsBigEndian;
		}
	}
	private bool NeedReverse
	{
		get
		{
			return IsLittleEndian ^ BitConverter.IsLittleEndian;
		}
	}

	public ushort ToUInt16(byte[] buffer, int start_index)
	{
		if (NeedReverse)
		{
			Array.Reverse(buffer, start_index, 2);
		}

		return BitConverter.ToUInt16(buffer, start_index);
	}

	public ushort ToUInt16(byte[] buffer)
	{
		return ToUInt16(buffer, 0);
	}

	public short ToInt16(byte[] buffer, int start_index)
	{
		if (NeedReverse)
		{
			Array.Reverse(buffer, start_index, 2);
		}

		return BitConverter.ToInt16(buffer, start_index);
	}

	public short ToInt16(byte[] buffer)
	{
		return ToInt16(buffer, 0);
	}

	public uint ToUInt32(byte[] buffer, int start_index)
	{
		if (NeedReverse)
		{
			Array.Reverse(buffer, start_index, 4);
		}

		return BitConverter.ToUInt32(buffer, start_index);
	}

	public uint ToUInt32(byte[] buffer)
	{
		return ToUInt32(buffer, 0);
	}

	public int ToInt32(byte[] buffer, int start_index)
	{
		if (NeedReverse)
		{
			Array.Reverse(buffer, start_index, 4);
		}

		return BitConverter.ToInt32(buffer, start_index);
	}

	public int ToInt32(byte[] buffer)
	{
		return ToInt32(buffer, 0);
	}

	public ulong ToUInt64(byte[] buffer, int start_index)
	{
		if (NeedReverse)
		{
			Array.Reverse(buffer, start_index, 8);
		}

		return BitConverter.ToUInt64(buffer, start_index);
	}

	public ulong ToUInt64(byte[] buffer)
	{
		return ToUInt64(buffer, 0);
	}

	public long ToInt64(byte[] buffer, int start_index)
	{
		if (NeedReverse)
		{
			Array.Reverse(buffer, start_index, 8);
		}

		return BitConverter.ToInt64(buffer, start_index);
	}

	public long ToInt64(byte[] buffer)
	{
		return ToInt64(buffer, 0);
	}

	public byte[] GetBytes(ushort value)
	{
		byte[] ret = BitConverter.GetBytes(value);
		if (NeedReverse)
		{
			Array.Reverse(ret);
		}

		return ret;
	}

	public byte[] GetBytes(short value)
	{
		byte[] ret = BitConverter.GetBytes(value);
		if (NeedReverse)
		{
			Array.Reverse(ret);
		}

		return ret;
	}

	public byte[] GetBytes(uint value)
	{
		byte[] ret = BitConverter.GetBytes(value);
		if (NeedReverse)
		{
			Array.Reverse(ret);
		}

		return ret;
	}

	public byte[] GetBytes(int value)
	{
		byte[] ret = BitConverter.GetBytes(value);
		if (NeedReverse)
		{
			Array.Reverse(ret);
		}

		return ret;
	}

	public byte[] GetBytes(ulong value)
	{
		byte[] ret = BitConverter.GetBytes(value);
		if (NeedReverse)
		{
			Array.Reverse(ret);
		}

		return ret;
	}

	public byte[] GetBytes(long value)
	{
		byte[] ret = BitConverter.GetBytes(value);
		if (NeedReverse)
		{
			Array.Reverse(ret);
		}

		return ret;
	}
}
