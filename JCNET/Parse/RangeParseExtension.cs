namespace JCNET.Parse;
public static class RangeParseExtension
{
	public static (bool success, string left, string right) ParseRange(this string value)
	{
		int index = value.IndexOf('-');
		if (index < 0)
		{
			// 找不到横杠，直接返回失败
			return (false, string.Empty, string.Empty);
		}

		try
		{
			return (true, value[0..index], value[(index + 1)..]);
		}
		catch
		{
			return (false, string.Empty, string.Empty);
		}
	}
}
