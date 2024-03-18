using System.Text;

namespace HttpServerLib;
public class ChunkWriter
{
	public ChunkWriter(Stream dstStream)
	{
		_dstStream = dstStream;
	}

	private readonly Stream _dstStream;
	private readonly byte[] _read_buf = new byte[1024];

	public async Task WriteAsync(Stream src_stream, CancellationToken token)
	{
		int i = 0;
		while (!token.IsCancellationRequested)
		{
			int readCount = await src_stream.ReadAsync(_read_buf);
			if (readCount == 0)
			{
				break;
			}

			Console.WriteLine($"第{i++}次分块");
			await WriteAsync(_read_buf, 0, readCount);
		}
	}

	public async Task WriteAsync(byte[] buf, int offset, int count)
	{
		string length = $"{count:x}\r\n";
		await _dstStream.WriteAsync(Encoding.UTF8.GetBytes(length));
		await _dstStream.WriteAsync(buf, offset, count);
		await _dstStream.WriteAsync(CRLF);
	}

	public async Task WriteTrailerAsync()
	{
		await _dstStream.WriteAsync(Trailer);
	}

	public static byte CR { get; } = 13;
	public static byte LF { get; } = 10;
	public static byte[] CRLF { get; } = new byte[2] { 13, 10 };

	/// <summary>
	/// 0\r\n\r\n
	/// </summary>
	public static byte[] Trailer { get; } = new byte[5] { 48, 13, 10, 13, 10 };
}
