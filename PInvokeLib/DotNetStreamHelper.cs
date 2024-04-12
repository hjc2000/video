using System.Runtime.InteropServices;

namespace PInvokeLib;

/// <summary>
///		帮助 C++ 使用 C# 的 Stream
/// </summary>
public partial class DotNetStreamHelper
{
	#region 对应C++中的函数的委托
	public unsafe delegate byte CanReadDelegate();
	public unsafe delegate byte CanWriteDelegate();
	public unsafe delegate byte CanSeekDelegate();

	public unsafe delegate long LengthDelegate();
	public unsafe delegate void SetLengthDelegate(long value);

	public unsafe delegate long ReadDelegate(byte* buffer, long offset, long count);
	public unsafe delegate void WriteDelegate(byte* buffer, long offset, long count);

	public unsafe delegate void FlushDelegate();
	public unsafe delegate void CloseDelegate();

	public unsafe delegate long PositionDelegate();
	public unsafe delegate void SetPositionDelegate(long value);

	public unsafe delegate byte ErrorDelegate();
	#endregion

	[LibraryImport("libPInvokeNativeHelper", EntryPoint = "CreateDotNetStream")]
	private static unsafe partial nuint CreateDotNetStream(
		CanReadDelegate can_read,
		CanWriteDelegate can_write,
		CanSeekDelegate can_seek,
		LengthDelegate length,
		SetLengthDelegate set_length,
		ReadDelegate read,
		WriteDelegate write,
		FlushDelegate flush,
		CloseDelegate close,
		PositionDelegate position,
		SetPositionDelegate set_position,
		ErrorDelegate error
	);

	[LibraryImport("libPInvokeNativeHelper", EntryPoint = "FreeDotNetStream")]
	private static unsafe partial void FreeDotNetStream(nuint cpp_obj);

	#region 与委托兼容的方法
	private byte CanRead()
	{
		try
		{
			return _stream.CanRead ? (byte)1 : (byte)0;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
			_error = true;
			return 0;
		}
	}

	private byte CanWrite()
	{
		try
		{
			return _stream.CanWrite ? (byte)1 : (byte)0;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
			_error = true;
			return 0;
		}
	}

	private byte CanSeek()
	{
		try
		{
			return _stream.CanSeek ? (byte)1 : (byte)0;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
			_error = true;
			return 0;
		}
	}

	private long Length()
	{
		try
		{
			return _stream.Length;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
			_error = true;
			return -1;
		}
	}

	private void SetLength(long value)
	{
		try
		{
			_stream.SetLength(value);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
			_error = true;
		}
	}

	private unsafe long Read(byte* buffer, long offset, long count)
	{
		try
		{
			Span<byte> span = new(buffer + offset, (int)count);
			return _stream.Read(span);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
			_error = true;
			return -1;
		}
	}

	private unsafe void Write(byte* buffer, long offset, long count)
	{
		try
		{
			Span<byte> span = new(buffer + offset, (int)count);
			_stream.Write(span);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
			_error = true;
		}
	}

	private void Flush()
	{
		try
		{
			_stream.Flush();
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
			_error = true;
		}
	}

	private void Close()
	{
		try
		{
			_stream.Close();
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
			_error = true;
		}
	}

	private long Position()
	{
		try
		{
			return _stream.Position;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
			_error = true;
			return -1;
		}
	}

	private void SetPosition(long value)
	{
		try
		{
			_stream.Position = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
			_error = true;
		}
	}

	private byte Error()
	{
		return _error ? (byte)1 : (byte)0;
	}
	#endregion
}

public partial class DotNetStreamHelper : IDisposable
{
	public DotNetStreamHelper(Stream stream)
	{
		_stream = stream;
		unsafe
		{
			CppObjRawPtr = CreateDotNetStream(
				CanRead,
				CanWrite,
				CanSeek,
				Length,
				SetLength,
				Read,
				Write,
				Flush,
				Close,
				Position,
				SetPosition,
				Error
			);
		}
	}

	~DotNetStreamHelper()
	{
		Dispose();
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

		FreeDotNetStream(CppObjRawPtr);
	}

	private Stream _stream;
	private bool _error = false;
	private nuint CppObjRawPtr { get; }
}