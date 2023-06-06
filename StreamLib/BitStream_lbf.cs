namespace StreamLib;

public class BitStream_lbf
{
	public BitStream_lbf(byte data)
	{
		_byte_data = data;
		_bitIndex = 7;
	}

	public bool ReadBit()
	{
		if (_bitIndex < 0)
		{
			throw new Exception("位流已到达尽头");
		}

		return BitView.ReadBit(_byte_data, _bitIndex--);
	}

	public byte ReadBits(int count)
	{
		if (count <= 0)
		{
			throw new ArgumentException("count 非法");
		}

		int endIndex = _bitIndex;
		_bitIndex -= count;
		return BitView.ReadBits(_byte_data, _bitIndex + 1, endIndex);
	}

	private readonly byte _byte_data;
	private int _bitIndex;
}
