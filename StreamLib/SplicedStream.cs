namespace StreamLib;
public class SplicedStream : Stream
{
	#region Stream 的实现
	public override bool CanRead => true;

	public override bool CanSeek => false;

	public override bool CanWrite => false;

	public override long Length => throw new NotImplementedException();

	public override long Position { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

	public override void Flush()
	{
		throw new NotImplementedException();
	}

	public override async Task<int> ReadAsync(byte[] buffer, int offset, int count, CancellationToken cancellationToken)
	{
		// 当前流为空，需要从队列中取出一个流
		if (_currentStream == null)
		{
			try
			{
				if (_streamQueue.Count == 0)
				{
					if (OnStreamQueueEmpty != null)
					{
						TaskCompletionSource tcs = new();
						OnStreamQueueEmpty?.Invoke(tcs);
						await tcs.Task;
					}
				}

				_currentStream = _streamQueue.Dequeue();
			}
			catch
			{
				// 队列中已经没有流了，返回 0，表示此流结束
				return 0;
			}
		}

		// 上面已经保证 _currentStream 不为空了，接下来只管读
		int haveRead = await _currentStream.ReadAsync(buffer, offset, count, cancellationToken);
		if (haveRead > 0)
		{
			return haveRead;
		}
		else
		{
			// 进入到这里只有可能是 haveRead == 0
			// 此流已经结束了，先释放此流
			_currentStream.Dispose();
			_currentStream = null;
			// 然后需要取出队列中的下一个流继续读取，这里使用递归
			return await ReadAsync(buffer, offset, count, cancellationToken);
		}
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
	public void PushBack(Stream stream)
	{
		_streamQueue.Enqueue(stream);
	}

	public event Action<TaskCompletionSource>? OnStreamQueueEmpty;
}
