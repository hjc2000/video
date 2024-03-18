namespace JCNET.流;

/// <summary>
/// 最高位先读的位流
/// </summary>
public class BitStream_lbf
{
	public BitStream_lbf(byte data)
	{
		BaseByte = data;
		_bitIndex = 7;
	}

	public bool ReadBit()
	{
		return _bitIndex < 0 ?
			throw new Exception("位流已到达尽头") :
			BitView.ReadBit(BaseByte, _bitIndex--);
	}

	public byte ReadBits(int count)
	{
		if (count <= 0)
		{
			throw new ArgumentException("count 非法");
		}

		int endIndex = _bitIndex;
		_bitIndex -= count;
		return BitView.ReadBits(BaseByte, _bitIndex + 1, endIndex);
	}

	public byte BaseByte { get; private set; }
	private int _bitIndex;
}
