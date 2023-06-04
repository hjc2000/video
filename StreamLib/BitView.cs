namespace StreamLib;
public class BitView
{
	public static bool ReadBit(ulong data, int bitIndex)
	{
		ulong mark = 1;
		mark <<= bitIndex;
		return (data & mark) != 0;
	}

	/// <summary>
	/// 读取从 startIndex 到 endIndex 的位，结果放到一个 ulong 中，
	/// startIndex 对应的位被放到 ulong 的第 0 位。返回的位中包含
	/// endIndex 对应的位，即：从 startIndex 到 endIndex 是一个闭区间
	/// </summary>
	/// <param name="data"></param>
	/// <param name="startIndex"></param>
	/// <param name="endIndex"></param>
	/// <returns></returns>
	public static ulong ReadBits(ulong data, int startIndex, int endIndex)
	{
		// 位长
		int length = endIndex - startIndex + 1;
		// 先左移，让右边出现 length 个 0 位，然后取反，这样右边就出现
		// length 个 1 位了，其它位都为 0
		ulong mark = ~(ulong.MaxValue << length);
		// 将 mark 中全 1 的部分移动到相应的窗口
		mark <<= startIndex;
		return (data & mark) >> startIndex;
	}
}
