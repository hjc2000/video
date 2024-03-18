namespace JCNET.流;
public static class StreamExtention
{
	/// <summary>
	/// 将流转化为字符串，不会消耗流，因为读取出字符串后会将 Position 设为 0.
	/// 注意，不是所有的流都支持调用此方法，因为有的流不支持设置 Position
	/// </summary>
	/// <param name="stream"></param>
	/// <returns></returns>
	public static async Task<string> ToStringWithoutConsumptionAsync(this Stream stream)
	{
		// 不要将 reader 释放，因为这会导致把基础流也给释放掉。本方法的用途是调试的时候打印一下
		// 流中的信息而不干扰原本的逻辑
		StreamReader reader = new(stream);
		long old_pos = stream.Position;
		string ret = await reader.ReadToEndAsync();
		stream.Position = old_pos;
		return ret;
	}

	/// <summary>
	///		将流的内容转化为字符串，会消费流。
	/// </summary>
	/// <param name="stream"></param>
	/// <returns></returns>
	public static async Task<string> ContentToStringAsync(this Stream stream)
	{
		StreamReader reader = new(stream);
		string ret = await reader.ReadToEndAsync();
		return ret;
	}

	/// <summary>
	///		将流的内容转化为字符串打印出来，会消费流。
	/// </summary>
	/// <param name="stream"></param>
	/// <returns></returns>
	public static async Task PrintAsync(this Stream stream)
	{
		Console.WriteLine(await stream.ContentToStringAsync());
	}

	/// <summary>
	///		利用 ToStringWithoutConsumptionAsync 将流转为字符串后打印。
	/// </summary>
	/// <param name="stream"></param>
	/// <returns></returns>
	public static async Task PrintStreamWithoutConsumptionAsync(this Stream stream)
	{
		try
		{
			Console.WriteLine(await stream.ToStringWithoutConsumptionAsync());
		}
		catch (Exception ex)
		{
			Console.WriteLine("此流可能不支持 seek 或不支持异步读取，PrintStreamWithoutConsumptionAsync 失败");
			Console.WriteLine(ex);
		}
	}
}
