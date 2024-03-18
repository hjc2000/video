namespace JCNET;

public static class Slice
{
	/// <summary>
	///	* 对字符串进行切片，从指定的第一个字符开始到指定的最后一个字符（闭区间，即
	/// 切下来的子字符串包含开始字符和结束字符）<br></br>
	///	* 指定的最后一个字符是从字符串末尾开始查找的，所以切下来的字符串片段会是最长的，所以叫Max
	///	* 出现异常会返回 null。异常的情况包括：不存在 start 或 end 指定的字符、
	///	end 指定的字符在 start 指定的字符前面
	/// </summary>
	/// <param name="str"></param>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <returns></returns>
	public static string? SliceMaxOn(this string str, char start, char end)
	{
		int startIndex = str.IndexOf(start);
		int endIndex = str.LastIndexOf(end);
		try
		{
			return str.Substring(startIndex, endIndex - startIndex + 1);
		}
		catch
		{
			return null;
		}
	}

	/// <summary>
	///	* 对字符串进行切片，从指定的第一个字符串开始到指定的最后一个字符串（闭区间，即
	/// 切下来的子字符串包含开始字符串和结束字符串）<br></br>
	///	* 指定的结束字符串是从字符串末尾开始查找的，所以切下来的字符串片段会是最长的，所以叫Max
	///	* 出现异常会返回 null。异常的情况包括：不存在 start 或 end 指定的字符串、
	///	end 指定的字符串在 start 指定的字符串前面或者两个字符串出现重叠
	/// </summary>
	/// <param name="str"></param>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <returns></returns>
	public static string? SliceMaxOn(this string str, string start, string end)
	{
		int startIndex = str.IndexOf(start);
		int endIndex = str.LastIndexOf(end);
		try
		{
			return str.Substring(startIndex, endIndex - startIndex + end.Length);
		}
		catch
		{
			return null;
		}
	}

	/// <summary>
	/// 切片不包含指定的字符（开区间）
	/// </summary>
	/// <param name="str"></param>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <returns></returns>
	public static string? SliceMaxBetween(this string str, char start, char end)
	{
		int startIndex = str.IndexOf(start);
		int endIndex = str.LastIndexOf(end);
		try
		{
			return str.Substring(startIndex + 1, endIndex - startIndex - 1);
		}
		catch
		{
			return null;
		}
	}

	/// <summary>
	/// 切片不包含指定的字符（开区间）
	/// </summary>
	/// <param name="str"></param>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <returns></returns>
	public static string? SliceMaxBetween(this string str, string start, string end)
	{
		int startIndex = str.IndexOf(start);
		int endIndex = str.LastIndexOf(end);
		try
		{
			return str.Substring(startIndex + start.Length, endIndex - startIndex - end.Length);
		}
		catch
		{
			return null;
		}
	}
}