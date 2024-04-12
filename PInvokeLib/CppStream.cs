using System.Runtime.InteropServices;

namespace PInvokeLib;

public partial class CppStream
{
	[LibraryImport("PInvokeNativeHelper", EntryPoint = "GetFileStream")]
	private static unsafe partial nuint GetFileStream(nuint std_string);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "FreeStream")]
	private static unsafe partial void FreeStream(nuint stream);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "Stream_CanRead")]
	private static unsafe partial byte Stream_CanRead(nuint stream);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "Stream_CanWrite")]
	private static unsafe partial byte Stream_CanWrite(nuint stream);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "Stream_CanSeek")]
	private static unsafe partial byte Stream_CanSeek(nuint stream);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "Stream_Length")]
	private static unsafe partial long Stream_Length(nuint stream);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "Stream_SetLength")]
	private static unsafe partial void Stream_SetLength(nuint stream, long value);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "Stream_Read")]
	private static unsafe partial long Stream_Read(nuint stream, byte* dst_buffer, long offset, long count);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "Stream_Write")]
	private static unsafe partial void Stream_Write(nuint stream, byte* dst_buffer, long offset, long count);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "Stream_Flush")]
	private static unsafe partial void Stream_Flush(nuint stream);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "Stream_Close")]
	private static unsafe partial void Stream_Close(nuint stream);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "Stream_Position")]
	private static unsafe partial long Stream_Position(nuint stream);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "Stream_SetPosition")]
	private static unsafe partial void Stream_SetPosition(nuint stream, long value);
}

/// <summary>
///		用来读取 C++ 中的流
/// </summary>
/// <param name="stream">C++ 中流对象的指针</param>
public partial class CppStream(nuint stream) : Stream
{
	public static CppStream GetFileStream(string url)
	{
		CppString std_str = new()
		{
			Value = url
		};
		string str = std_str.ToString();
		nuint fs = GetFileStream(std_str.RawPtr);
		return new CppStream(fs);
	}

	private bool _disposed = false;
	protected override void Dispose(bool disposing)
	{
		base.Dispose(disposing);
		if (_disposed)
		{
			return;
		}

		_disposed = true;
		if (disposing)
		{
			// 垃圾回收时可能不存活的对象
		}

		// 垃圾回收时一定还存活的对象
		FreeStream(stream);
	}

	public override void Flush()
	{
		Stream_Flush(stream);
	}

	public override int Read(byte[] buffer, int offset, int count)
	{
		unsafe
		{
			fixed (byte* ptr = buffer)
			{
				return (int)Stream_Read(stream, ptr, offset, count);
			}
		}
	}

	public override long Seek(long offset, SeekOrigin origin)
	{
		switch (origin)
		{
		case SeekOrigin.Begin:
			{
				Position = offset;
				break;
			}
		case SeekOrigin.Current:
			{
				Position += offset;
				break;
			}
		case SeekOrigin.End:
			{
				Position = Length + offset;
				break;
			}
		default:
			{
				throw new ArgumentOutOfRangeException(nameof(origin), "Invalid seek origin.");
			}
		}

		// 确保新位置不超出流的边界
		if (Position < 0)
		{
			Position = 0;
			throw new IOException("Seeking before the beginning of the stream.");
		}
		else if (Position > Length)
		{
			Position = Length;
			throw new IOException("Seeking beyond the end of the stream.");
		}

		return Position;
	}

	public override void SetLength(long value)
	{
		Stream_SetLength(stream, value);
	}

	public override void Write(byte[] buffer, int offset, int count)
	{
		unsafe
		{
			fixed (byte* ptr = buffer)
			{
				Stream_Write(stream, ptr, offset, count);
			}
		}
	}

	public override bool CanRead
	{
		get
		{
			return Stream_CanRead(stream) != 0;
		}
	}
	public override bool CanSeek
	{
		get
		{
			return Stream_CanSeek(stream) != 0;
		}
	}
	public override bool CanWrite
	{
		get
		{
			return Stream_CanWrite(stream) != 0;
		}
	}
	public override long Length
	{
		get
		{
			return Stream_Length(stream);
		}
	}
	public override long Position
	{
		get
		{
			return Stream_Position(stream);
		}
		set
		{
			Stream_SetPosition(stream, value);
		}
	}
}
