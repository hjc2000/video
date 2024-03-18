#pragma once
#include<Exception.h>

namespace A3
{
	/// <summary>
	///		写 A3 的内存失败。
	/// </summary>
	class WriteMemoryException :public jc::Exception
	{
	public:
		WriteMemoryException() :Exception(__func__) {}
		WriteMemoryException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		WriteMemoryException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		读 A3 的内存失败。
	/// </summary>
	class ReadMemoryException :public jc::Exception
	{
	public:
		ReadMemoryException() :Exception(__func__) {}
		ReadMemoryException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		ReadMemoryException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		芯片状态异常。
	///		* 要求芯片处于运行状态，但是不是。
	///		* 要求芯片处于停止状态，但是不是。
	///		* 总之就是要求芯片处于某种状态或模式，但是不对。
	/// </summary>
	class ChipStatusException :public jc::Exception
	{
	public:
		ChipStatusException() :Exception(__func__) {}
		ChipStatusException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		ChipStatusException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		A3 向主机返回的数据是非法的或未定义的。
	/// </summary>
	class A3DataException :public jc::Exception
	{
	public:
		A3DataException() :Exception(__func__) {}
		A3DataException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		A3DataException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		USB 传输异常。
	/// </summary>
	class UsbException :public jc::Exception
	{
	public:
		UsbException() :Exception(__func__) {}
		UsbException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		UsbException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};
}
