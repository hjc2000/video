namespace StreamLib;
public class BitView
{
	public static ulong ReadBit(ulong data, int bitIndex)
	{
		ulong mark = 1;
		mark <<= bitIndex;
		return (data & mark) >> bitIndex;
	}

	public static ulong ReadBits(ulong data, int startIndex, int length)
	{
		// 先左移，让右边出现 length 个 0 位，然后取反，这样右边就出现
		// length 个 1 位了，其它位都为 0
		ulong mark = ~(ulong.MaxValue << length);
		mark <<= startIndex;
		return (data & mark) >> startIndex;
	}
}
