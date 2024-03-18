using System.Net.WebSockets;

namespace HttpServerLib;

/// <summary>
///		接管 WebSocket 对象，然后可以用流的方式读写。
///		
///		<br/> WriteAsync 不会发送消息结束标志，要调用 FlushAsync 才能发送消息结束标志。
///		
///		<br/> 读取时，读取到 websocket 消息结尾，就认为本流就结束了，后面再调用 ReadAsync 永远都会返回 0，除非使用 ResetReadStream
///		方法重置 “读取流结束标志” 才能继续读取下一个消息。
/// </summary>
public class WebSocketStream : Stream
{
	public WebSocketStream(WebSocket socket)
	{
		_socket = socket;
	}

	private readonly WebSocket _socket;

	/// <summary>
	///		向 websocket 发送结束消息
	/// </summary>
	/// <param name="cancellationToken"></param>
	/// <returns></returns>
	public override async Task FlushAsync(CancellationToken cancellationToken)
	{
		await _socket.SendAsync(Array.Empty<byte>(), WebSocketMessageType.Binary, true, cancellationToken);
		Console.WriteLine("冲洗缓冲区完成");
	}

	/// <summary>
	///		SyncReadingFrame 方法专用的缓冲区，只能写，不能读。这就是一个垃圾堆，所以不做
	///		线程同步也没事，数据损坏就损坏了。
	/// </summary>
	private static readonly byte[] _dustbin = new byte[1024];

	/// <summary>
	///		对读取流进行帧同步。
	///		
	///		<br/> 调用 ReadAsync 读取一个新的帧之前一定要调用本函数。本函数可以清理流中的垃圾数据。
	///		此外，本类对象刚刚构造后，调用 ReadAsync 会永远返回 0，一定要调用本函数才可以开始读取。
	/// </summary>
	/// <param name="cancellationToken"></param>
	/// <returns></returns>
	public async Task SyncReadingFrame(CancellationToken cancellationToken = default)
	{
		/* 流中如果残留着垃圾数据，在下面这个循环中会读取直到流结尾，以清理垃圾数据，从而使帧重新同步。
		 * 
		 * _last_read_is_end_of_msg 在本类构造的时候就是 true，所以如果是刚构造就调用本函数，下面的
		 * ReadAsync 会直接返回 0
		 */
		while (true)
		{
			int have_read = await ReadAsync(_dustbin, cancellationToken);
			if (have_read == 0)
			{
				break;
			}
		}

		// 赋值为 false 后就可以调用 ReadAsync 函数读取下一个帧了
		_last_read_is_end_of_msg = false;
	}

	/// <summary>
	///		本流一开始创建就是流结束，必须调用 SyncReadingFrame 才能读取。
	/// </summary>
	private bool _last_read_is_end_of_msg = true;

	/// <summary>
	///		读取 websocket。如果上一次读取是 websocket 的结束消息，会直接返回 0. 这时候需要调用
	///		SyncReadingFrame 方法才能读取下一个帧，否则永远都是直接返回 0.
	///		
	///		本类对象刚构造，第一次读取时也要先调用 SyncReadingFrame 方法。
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="cancellationToken"></param>
	/// <returns></returns>
	public override async ValueTask<int> ReadAsync(Memory<byte> buffer, CancellationToken cancellationToken = default)
	{
		if (_last_read_is_end_of_msg)
		{
			return 0;
		}

		while (true)
		{
			ValueWebSocketReceiveResult result = await _socket.ReceiveAsync(buffer, cancellationToken);
			_last_read_is_end_of_msg = result.EndOfMessage;
			return result.Count;
		}
	}

	public override async Task<int> ReadAsync(byte[] buffer, int offset, int count, CancellationToken cancellationToken)
	{
		Memory<byte> memory = new(buffer, offset, count);
		return await ReadAsync(memory, cancellationToken);
	}

	/// <summary>
	///		向 websocket 发送数据
	/// </summary>
	/// <remarks>
	///		WriteAsync 的所有重载发送数据都不会设置 websocket 的消息结尾标志。想要通知接收方消息
	///		结束了，请调用 FlushAsync 方法发送一个空包和消息结束标志。
	/// </remarks>
	/// <param name="buffer"></param>
	/// <param name="cancellationToken"></param>
	/// <returns></returns>
	public override async ValueTask WriteAsync(ReadOnlyMemory<byte> buffer, CancellationToken cancellationToken = default)
	{
		await _socket.SendAsync(buffer, WebSocketMessageType.Binary, false, cancellationToken);
	}

	/// <summary>
	///		向 websocket 发送数据
	/// </summary>
	/// <remarks>
	///		WriteAsync 的所有重载发送数据都不会设置 websocket 的消息结尾标志。想要通知接收方消息
	///		结束了，请调用 FlushAsync 方法发送一个空包和消息结束标志。
	/// </remarks>
	/// <param name="buffer"></param>
	/// <param name="offset"></param>
	/// <param name="count"></param>
	/// <param name="cancellationToken"></param>
	/// <returns></returns>
	public override async Task WriteAsync(byte[] buffer, int offset, int count, CancellationToken cancellationToken)
	{
		Memory<byte> memory = new(buffer, offset, count);
		await WriteAsync(memory, cancellationToken);
	}

	#region 不实现的功能
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

	public override long Length
	{
		get { throw new NotSupportedException(); }
	}

	public override long Position
	{
		get { throw new NotSupportedException(); }
		set { throw new NotSupportedException(); }
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
			return true;
		}
	}
}
