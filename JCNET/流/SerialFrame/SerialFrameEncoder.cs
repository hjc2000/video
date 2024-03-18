namespace JCNET.流.SerialFrame;

/// <summary>
///		用于将字节数组打包为 1 个帧。用来在串行通信中区分数据边界。
///		
///		帧的开始标志为 254，结束标志为 255. 数据场中的数据如果与开始标志和结束标志冲突了，
///		需要对数据场中的数据进行字节填充。接收端需要解除填充，还原出数据场的数据。
///		
///		填充字节选定为 111. 接收端接收到 111 后：
///		1. 如果后一个字节还是是 111，则表示这两个字节传输的是一个字节的数据：111.
///		2. 如果后一个字节是 0，则表示这 2 个字节传输的是 1 个字节的数据：254.
///		3. 如果后一个字节是 1，则表示这 2 个字节传输的是 1 个字节的数据：255.
/// </summary>
public class SerialFrameEncoder : Stream
{
	/// <summary>
	/// 
	/// </summary>
	/// <param name="dst_stream">数据目的地。编码后的数据被写入此流。</param>
	public SerialFrameEncoder(Stream dst_stream)
	{
		_dst_stream = dst_stream;
	}

	/// <summary>
	///		数据目的地。编码后的数据被写入此流。
	/// </summary>
	private Stream _dst_stream;

	public override async Task FlushAsync(CancellationToken cancellationToken)
	{
		await _dst_stream.FlushAsync(cancellationToken);
	}

	public override void Flush()
	{
		_dst_stream.Flush();
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

	private MemoryStream _mstream = new();

	public override async Task WriteAsync(byte[] buffer, int offset, int count, CancellationToken cancellationToken)
	{
		_mstream.SetLength(0);
		_mstream.WriteByte(254);// 开始标志
		for (int i = offset; i < offset + count; i++)
		{
			switch (buffer[i])
			{
			case 111:
			case 254:
			case 255:
				{
					_mstream.WriteByte(111);// 填充字节
					_mstream.WriteByte(buffer[i]);
					break;
				}
			default:
				{
					_mstream.WriteByte(buffer[i]);
					break;
				}
			}
		}

		_mstream.WriteByte(255);// 结束标志
		_mstream.Position = 0;
		await _mstream.CopyToAsync(_dst_stream, cancellationToken);
		_mstream.SetLength(0);
	}

	public override void Write(byte[] buffer, int offset, int count)
	{
		_dst_stream.WriteByte(254);// 开始标志
		for (int i = offset; i < offset + count; i++)
		{
			switch (buffer[i])
			{
			case 111:
			case 254:
			case 255:
				{
					_dst_stream.WriteByte(111);// 填充字节
					_dst_stream.WriteByte(buffer[i]);
					break;
				}
			default:
				{
					_dst_stream.WriteByte(buffer[i]);
					break;
				}
			}
		}

		_dst_stream.WriteByte(255);// 结束标志
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
		get { throw new NotSupportedException(); }
	}

	public override long Position
	{
		get { throw new NotSupportedException(); }
		set { throw new NotSupportedException(); }
	}
}
