namespace JCNET.流;

public class JoinedStream : Stream
{
	private bool _disposed = false;
	public override async ValueTask DisposeAsync()
	{
		if (_disposed)
		{
			return;
		}

		_disposed = true;
		GC.SuppressFinalize(this);

		await base.DisposeAsync();
		if (_currentStream != null)
		{
			await _currentStream.DisposeAsync();
		}
	}

	public override bool CanRead
	{
		get
		{
			return true;
		}
	}

	public override bool CanSeek
	{
		get
		{
			return false;
		}
	}

	public override bool CanWrite
	{
		get
		{
			return false;
		}
	}

	private long _position;
	public override long Position
	{
		get
		{
			return _position;
		}

		set
		{
			throw new NotImplementedException();
		}
	}

	/// <summary>
	///		尝试从队列和 IJoinedStreamCurrentStreamEndHandler 中获取新的流。如果两者都没有，返回 null
	/// </summary>
	/// <returns></returns>
	private async Task<Stream?> TryGetNewStream()
	{
		try
		{
			// 如果队列空了，先触发 OnCurrentStreamEnd 然后再尝试退队
			if (_streamQueue.Count == 0 && CurrentStreamEndHandler != null)
			{
				await CurrentStreamEndHandler.OnCurrentStreamEnd();
			}

			// 退队
			lock (_streamQueue)
			{
				/* 如果 OnCurrentStreamEnd 后有流入队，则退队成功，否则退队失败，抛出异常*/
				return _streamQueue.Dequeue();
			}
		}
		catch
		{
			// 队列中已经没有流了，退队失败
			return null;
		}
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="offset"></param>
	/// <param name="count"></param>
	/// <param name="cancellationToken"></param>
	/// <returns></returns>
	/// <exception cref="TaskCanceledException"></exception>
	public override async Task<int> ReadAsync(byte[] buffer, int offset, int count, CancellationToken cancellationToken)
	{
		while (!cancellationToken.IsCancellationRequested)
		{
			// 当前流为空，需要加载一个新的流
			if (_currentStream == null)
			{
				_currentStream = await TryGetNewStream();

				// 尝试加载后仍然为空，返回 0，表示 JoinedStream 到达尽头
				if (_currentStream == null)
				{
					return 0;
				}
			}

			// 执行到这里说明 _currentStream 不为空。
			int haveRead = await _currentStream.ReadAsync(buffer, cancellationToken);
			if (haveRead == 0)
			{
				// 当前流已经结束了，先释放此流
				_currentStream.Dispose();
				_currentStream = null;
				continue;
			}

			_position += haveRead;
			return haveRead;
		}

		throw new TaskCanceledException();
	}

	public override async ValueTask<int> ReadAsync(Memory<byte> buffer, CancellationToken cancellationToken = default)
	{
		// 当前流为空，需要加载一个新的流
		if (_currentStream == null)
		{
			_currentStream = await TryGetNewStream();
			// 尝试加载后仍然为空，返回 0，表示 JoinedStream 到达尽头
			if (_currentStream == null)
			{
				return 0;
			}
		}

		// 上面已经保证 _currentStream 不为空了（为空已经返回 0 了，执行不到这里），接下来只管读
		int haveRead = await _currentStream.ReadAsync(buffer, cancellationToken);
		if (haveRead > 0)
		{
			_position += haveRead;
			return haveRead;
		}
		else
		{
			// 进入到这里只有可能是 haveRead == 0
			// 此流已经结束了，先释放此流
			_currentStream.Dispose();
			_currentStream = null;
			// 然后需要取出队列中的下一个流继续读取，这里使用递归
			return await ReadAsync(buffer, cancellationToken);
		}
	}

	#region 不实现的功能
	public override long Length
	{
		get
		{
			throw new NotImplementedException();
		}
	}

	public override void Flush()
	{
		throw new NotImplementedException();
	}
	/// <summary>
	/// 本流不支持同步读取
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="offset"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	/// <exception cref="NotImplementedException"></exception>
	public override int Read(byte[] buffer, int offset, int count)
	{
		throw new NotImplementedException();
	}
	public override long Seek(long offset, SeekOrigin origin)
	{
		throw new NotImplementedException();
	}
	public override void SetLength(long value)
	{
		throw new NotImplementedException();
	}
	public override void Write(byte[] buffer, int offset, int count)
	{
		throw new NotImplementedException();
	}
	#endregion

	private readonly Queue<Stream> _streamQueue = new();
	private Stream? _currentStream = null;

	/// <summary>
	/// 向队列的末尾添加流
	/// </summary>
	/// <param name="stream"></param>
	public void AppendStream(Stream stream)
	{
		lock (_streamQueue)
		{
			_streamQueue.Enqueue(stream);
		}
	}

	public IJoinedStreamCurrentStreamEndHandler? CurrentStreamEndHandler { get; set; }
}

/// <summary>
///		用于向 JoinedStream 提供流。
/// </summary>
public interface IJoinedStreamCurrentStreamEndHandler
{
	/// <summary>
	///		JoinedStream 在当前流读到尽头的时候就会调用此函数。在此函数中需要调用 JoinedStream
	///		的 AppendStream 函数，否则本函数返回后 JoinedStream 就会结束。
	/// </summary>
	/// <returns></returns>
	Task OnCurrentStreamEnd();
}