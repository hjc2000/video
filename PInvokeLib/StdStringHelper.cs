using System.Runtime.InteropServices;
using System.Text;

namespace PInvokeLib;

public partial class StdStringHelper
{
	[LibraryImport("PInvokeNativeHelper", EntryPoint = "CreateEmptyString")]
	private static unsafe partial nuint CreateEmptyString();

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "String_GetSize")]
	private static unsafe partial int String_GetSize(nuint std_string);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "String_GetBuffer")]
	private static unsafe partial byte* String_GetBuffer(nuint std_string);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "String_Set")]
	private static unsafe partial void String_Set(nuint std_string, byte* char_arr, int size);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "FreeStdString")]
	private static unsafe partial void FreeStdString(nuint std_string);
}

public partial class StdStringHelper : IDisposable
{
	#region 生命周期
	public StdStringHelper()
	{
		RawPtr = CreateEmptyString();
	}

	public StdStringHelper(nuint std_string)
	{
		RawPtr = std_string;
	}

	~StdStringHelper()
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

		FreeStdString(RawPtr);
	}
	#endregion

	public nuint RawPtr { get; }

	/// <summary>
	///		获取字符串所占的字节数
	/// </summary>
	public int Size
	{
		get
		{
			return String_GetSize(RawPtr);
		}
	}

	/// <summary>
	///		底层的 std::string 的值。
	/// </summary>
	public string Value
	{
		get
		{
			return ToString();
		}
		set
		{
			byte[] char_arr = Encoding.UTF8.GetBytes(value);
			unsafe
			{
				fixed (byte* ptr = char_arr)
				{
					String_Set(RawPtr, ptr, char_arr.Length);
				}
			}
		}
	}

	public override string ToString()
	{
		int size = Size;
		if (size <= 0)
		{
			return string.Empty;
		}

		unsafe
		{
			byte* cstring_buffer = String_GetBuffer(RawPtr);
			Span<byte> char_arr = new(cstring_buffer, size);
			return Encoding.UTF8.GetString(char_arr);
		}
	}
}