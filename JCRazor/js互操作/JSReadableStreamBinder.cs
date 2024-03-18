using JCNET.流;
using Microsoft.JSInterop;

namespace JCRazor.js互操作;

public class JSReadableStreamBinder : Stream, IJSObjectProjection, IJoinedStreamCurrentStreamEndHandler
{
	/// <summary>
	/// 
	/// </summary>
	/// <param name="jsrt"></param>
	/// <param name="readable_stream">js 中的 ReadableStream 对象。</param>
	public JSReadableStreamBinder(IJSRuntime jsrt, IJSObjectReference readable_stream)
	{
		_joined_stream.CurrentStreamEndHandler = this;
		_ = InitAsync(jsrt, readable_stream);
	}

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

		// 如果任务没完成，直接取消
		await _joined_stream.DisposeAsync();
		_joined_stream.CurrentStreamEndHandler = null;
		_init_tcs.TrySetCanceled();
		if (_init_tcs.Task.IsCompletedSuccessfully)
		{
			await Projection.DisposeAsync();
		}
	}

	private TaskCompletionSource _init_tcs = new();
	private async Task InitAsync(IJSRuntime jsrt, IJSObjectReference readable_stream)
	{
		await using JSModule module = new(jsrt, "./_content/JCRazor/ReadableStreamBinder.js");
		Projection = await module.InvokeAsync<IJSObjectReference>("ReadableStreamBinder.create", readable_stream);
		_init_tcs.TrySetResult();
	}

	/// <summary>
	///		获取流的大小。如果此流不支持获取大小则返回值为 -1
	/// </summary>
	/// <returns></returns>
	public async Task<long> GetLengthAsync()
	{
		await _init_tcs.Task;
		_length = await Projection.InvokeAsync<long>("get_stream_size");
		return _length;
	}

	/// <summary>
	///		用 GetLengthAsync 方法初始化 Length 属性。
	/// </summary>
	/// <returns></returns>
	public async Task InitLengthAsync()
	{
		_ = await GetLengthAsync();
	}

	/// <summary>
	///		获取文件名
	/// </summary>
	/// <returns></returns>
	public async Task<string> GetFileNameAsync()
	{
		await _init_tcs.Task;
		return await Projection.InvokeAsync<string>("get_file_name");
	}

	/// <summary>
	///		通过调用 js 的 ReadableStreamDefaultReader 的 read 方法来读取流
	/// </summary>
	/// <returns></returns>
	/// <exception cref="JSException">流到达尽头</exception>
	public async Task<byte[]> ReadAsync()
	{
		await _init_tcs.Task;
		try
		{
			// js 流的一部分会被读成字节数组，通过 IJSStreamReference 流式传输到 C# 
			byte[] buffer = await Projection.InvokeAsync<byte[]>("read");
			return buffer;
		}
		catch (Exception ex)
		{
			throw new JSException($"{ex.Message}");
		}
	}

	public IJSObjectReference Projection { get; private set; } = default!;

	private JoinedStream _joined_stream = new();
	async Task IJoinedStreamCurrentStreamEndHandler.OnCurrentStreamEnd()
	{
		try
		{
			MemoryStream mstream = new(await ReadAsync());
			_joined_stream.AppendStream(mstream);
		}
		catch { }
	}

	public override async ValueTask<int> ReadAsync(Memory<byte> buffer, CancellationToken cancellationToken = default)
	{
		int haveRead = await _joined_stream.ReadAsync(buffer, cancellationToken);
		_position += haveRead;
		if (_length == -1)
		{
			await InitLengthAsync();
		}

		PositionChangeEventArgs arg = new()
		{
			Position = _position,
			Length = _length
		};

		PositionChangedEvent?.Invoke(arg);
		return haveRead;
	}

	#region 不支持的功能
	public override void Flush()
	{
		throw new NotSupportedException();
	}

	public override int Read(byte[] buffer, int offset, int count)
	{
		throw new NotSupportedException();
	}

	public override long Seek(long offset, SeekOrigin origin)
	{
		throw new NotSupportedException();
	}

	public override void SetLength(long value)
	{
		throw new NotSupportedException();
	}

	public override void Write(byte[] buffer, int offset, int count)
	{
		throw new NotSupportedException();
	}
	#endregion

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

	private long _length = -1;
	/// <summary>
	///		本属性需要先调用 InitLengthAsync 方法或者 GetLengthAsync 方法后才能使用。
	/// </summary>
	public override long Length
	{
		get
		{
			return _length;
		}
	}

	private long _position = 0;
	public override long Position
	{
		get
		{
			return _position;
		}
		set
		{
			throw new NotSupportedException();
		}
	}

	public event Action<PositionChangeEventArgs>? PositionChangedEvent;
}

public struct PositionChangeEventArgs
{
	public long Position { get; set; }
	public long Length { get; set; }
}