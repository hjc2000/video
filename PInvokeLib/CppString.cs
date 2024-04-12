using System.Runtime.InteropServices;
using System.Text;

namespace PInvokeLib;

public partial class CppString
{
	[LibraryImport("libPInvokeNativeHelper", EntryPoint = "CreateEmptyString")]
	private static unsafe partial nuint CreateEmptyString();

	[LibraryImport("libPInvokeNativeHelper", EntryPoint = "String_GetSize")]
	private static unsafe partial int String_GetSize(nuint std_string);

	[LibraryImport("libPInvokeNativeHelper", EntryPoint = "String_GetBuffer")]
	private static unsafe partial byte* String_GetBuffer(nuint std_string);

	[LibraryImport("libPInvokeNativeHelper", EntryPoint = "String_Set")]
	private static unsafe partial void String_Set(nuint std_string, byte* char_arr, int size);

	[LibraryImport("libPInvokeNativeHelper", EntryPoint = "FreeStdString")]
	private static unsafe partial void FreeStdString(nuint std_string);
}

public partial class CppString : IDisposable
{
	#region 生命周期
	/// <summary>
	///		会在 C++ 侧创建一个空的 std::string 用于绑定。
	/// </summary>
	public CppString()
	{
		CppObjRawPtr = CreateEmptyString();
	}

	/// <summary>
	///		接管一个 std::string 指针。本对象释放时会执行 C++ 侧的 delete 删除它。
	/// </summary>
	/// <param name="std_string"></param>
	public CppString(nuint std_string)
	{
		CppObjRawPtr = std_string;
	}

	~CppString()
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

		FreeStdString(CppObjRawPtr);
	}
	#endregion

	public nuint CppObjRawPtr { get; }

	/// <summary>
	///		获取字符串所占的字节数
	/// </summary>
	public int Size
	{
		get
		{
			return String_GetSize(CppObjRawPtr);
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
					String_Set(CppObjRawPtr, ptr, char_arr.Length);
				}
			}
		}
	}

	/// <summary>
	///		从 std::string 缓冲区读取内容到 C# 的缓冲区，然后转成 C# 的 string。
	/// </summary>
	/// <returns></returns>
	public override string ToString()
	{
		int size = Size;
		if (size <= 0)
		{
			return string.Empty;
		}

		unsafe
		{
			byte* cstring_buffer = String_GetBuffer(CppObjRawPtr);
			Span<byte> char_arr = new(cstring_buffer, size);
			return Encoding.UTF8.GetString(char_arr);
		}
	}
}