using System.Threading.Channels;

namespace HttpServerLib;
/// <summary>
/// 接管 ChannelReader<byte>，像流一样读取其中的数据
/// </summary>
public class ChannelReaderStream : Stream
{
	public ChannelReaderStream(ChannelReader<byte> reader)
	{
		_ch_reader = reader;
	}

	private readonly ChannelReader<byte> _ch_reader;

	public override async ValueTask<int> ReadAsync(Memory<byte> buffer, CancellationToken cancellationToken = default)
	{
		int offset = 0;
		while (await _ch_reader.WaitToReadAsync(cancellationToken))
		{
			byte ret = await _ch_reader.ReadAsync(cancellationToken);
			buffer.Span[offset++] = ret;
			if (offset >= buffer.Length)
			{
				break;
			}
		}

		return offset;
	}

	public override async Task<int> ReadAsync(byte[] buffer, int offset, int count, CancellationToken cancellationToken)
	{
		Memory<byte> memory = new(buffer, offset, count);
		return await ReadAsync(memory, cancellationToken);
	}

	public override void Flush()
	{
		throw new NotImplementedException();
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
