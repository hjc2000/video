namespace JCNET;

public class LockGuard : IDisposable
{
	/// <summary>
	///		* semaphore 一旦使用本对象进行管理后，禁止私自 Release。<br/>
	///		* 构造本对象时需要使用 using 语句，这样才能保证 Dispose 方法被及时调用。 <br/>
	///		* 构造完本对象后，需要手动调用 semaphore 对象的 WaitAsync 方法。 <br/>
	/// </summary>
	/// <param name="semaphore"></param>
	public LockGuard(SemaphoreSlim semaphore)
	{
		_semaphore = semaphore;
	}

	private bool _disposed = false;
	public void Dispose()
	{
		if (_disposed)
		{
			return;
		}

		_disposed = true;
		GC.SuppressFinalize(this);

		if (_semaphore.CurrentCount > 0)
		{
			Console.WriteLine("_semaphore.CurrentCount > 0，根本没有加锁，检查看是不是忘记等待 _semaphore 了");
		}

		try
		{
			_semaphore.Release();
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	private SemaphoreSlim _semaphore;

	public async Task WaitAsync()
	{
		await _semaphore.WaitAsync();
	}

	public async Task WaitAsync(CancellationToken cancellationToken)
	{
		await _semaphore.WaitAsync(cancellationToken);
	}
}
