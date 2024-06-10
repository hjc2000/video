using System.Runtime.InteropServices;

namespace PInvokeLib;

/// <summary>
///		帮助 C++ 使用 C# 的 Stream
/// </summary>
public partial class DotNetStreamHelper
{
	#region 对应 C++ 中的函数的委托，与 C++ 兼容，供 C++ 调用
	public unsafe delegate byte CanReadDelegate();
	public unsafe delegate byte CanWriteDelegate();
	public unsafe delegate byte CanSeekDelegate();

	public unsafe delegate long LengthDelegate();
	public unsafe delegate void SetLengthDelegate(long value);

	public unsafe delegate int ReadDelegate(byte* buffer, int offset, int count);
	public unsafe delegate void WriteDelegate(byte* buffer, int offset, int count);

	public unsafe delegate void FlushDelegate();
	public unsafe delegate void CloseDelegate();

	public unsafe delegate long PositionDelegate();
	public unsafe delegate void SetPositionDelegate(long value);

	public unsafe delegate byte ErrorDelegate();
	#endregion

	/// <summary>
	///		在 C++ 侧创建一个 DotNetStream 对象并返回它的指针。
	/// </summary>
	/// <param name="can_read"></param>
	/// <param name="can_write"></param>
	/// <param name="can_seek"></param>
	/// <param name="length"></param>
	/// <param name="set_length"></param>
	/// <param name="read"></param>
	/// <param name="write"></param>
	/// <param name="flush"></param>
	/// <param name="close"></param>
	/// <param name="position"></param>
	/// <param name="set_position"></param>
	/// <param name="error"></param>
	/// <returns></returns>
	[LibraryImport("libpinvoke", EntryPoint = "CreateDotNetStream")]
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

	/// <summary>
	///		释放 C++ 侧的 DotNetStream 对象。
	/// </summary>
	/// <param name="cpp_obj"></param>
	[LibraryImport("libpinvoke", EntryPoint = "FreeDotNetStream")]
	private static unsafe partial void FreeDotNetStream(nuint cpp_obj);

	#region 将对 Stream 的操作函数包装成与供 C++ 调用的委托兼容的形式。
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

	private unsafe int Read(byte* buffer, int offset, int count)
	{
		try
		{
			Span<byte> span = new(buffer + offset, count);
			return _stream.Read(span);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
			_error = true;
			return -1;
		}
	}

	private unsafe void Write(byte* buffer, int offset, int count)
	{
		try
		{
			Span<byte> span = new(buffer + offset, count);
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
	public nuint CppObjRawPtr { get; }
}