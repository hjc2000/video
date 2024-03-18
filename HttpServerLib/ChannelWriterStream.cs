using System.Threading.Channels;

namespace HttpServerLib;
/// <summary>
/// 接管 ChannelWriter<byte>，像流一样写入
/// </summary>
public class ChannelWriterStream : Stream
{
	public ChannelWriterStream(ChannelWriter<byte> writer)
	{
		_writer = writer;
	}

	private readonly ChannelWriter<byte> _writer;

	public override void Flush()
	{
		_writer.Complete();
	}

	public override async Task FlushAsync(CancellationToken cancellationToken)
	{
		await Task.CompletedTask;
		Flush();
	}

	public override async ValueTask WriteAsync(ReadOnlyMemory<byte> buffer, CancellationToken cancellationToken = default)
	{
		for (int i = 0; i < buffer.Length; i++)
		{
			// 有时候会收到阻塞，例如网络不好，缓冲区满。所以等待可写时再写入
			if (await _writer.WaitToWriteAsync(cancellationToken))
			{
				await _writer.WriteAsync(buffer.Span[i], cancellationToken);
			}
			else
			{
				throw new Exception("无法再写入此 ChannelWriter");
			}
		}
	}

	public override async Task WriteAsync(byte[] buffer, int offset, int count, CancellationToken cancellationToken)
	{
		Memory<byte> memory = new(buffer, offset, count);
		await WriteAsync(memory, cancellationToken);
	}

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

	public override bool CanRead
	{
		get
		{
			return false;
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
	public override long Length
	{
		get { throw new NotImplementedException(); }
	}
	public override long Position
	{
		get { throw new NotImplementedException(); }
		set { throw new NotImplementedException(); }
	}
}
