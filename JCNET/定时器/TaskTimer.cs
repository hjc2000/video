namespace JCNET.定时器;

public static class TaskTimer
{
	/// <summary>
	///		以一定时间间隔执行 func。
	///		* 逻辑是执行完 func 后等待 interval，然后开始下一循环。
	/// </summary>
	/// <param name="func"></param>
	/// <param name="interval"></param>
	/// <param name="token"></param>
	public static void SetInterval(Func<Task> func, TimeSpan interval, CancellationToken token)
	{
		try
		{
			Task.Run(async () =>
			{
				while (!token.IsCancellationRequested)
				{
					await func();
					await Task.Delay(interval, token);
				}
			}, token);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	/// <summary>
	///		以一定时间间隔执行 func。
	///		* 逻辑是执行完 func 后等待 interval_in_milliseconds，然后开始下一循环。
	/// </summary>
	/// <param name="func"></param>
	/// <param name="interval_in_milliseconds"></param>
	/// <param name="token"></param>
	public static void SetInterval(Func<Task> func, int interval_in_milliseconds, CancellationToken token)
	{
		SetInterval(func, TimeSpan.FromMilliseconds(interval_in_milliseconds), token);
	}
}
