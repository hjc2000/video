#pragma once
#include<stdexcept>

namespace A3
{
	/// <summary>
	///		写 A3 的内存失败。
	/// </summary>
	class WriteMemoryException :public std::runtime_error
	{
	public:
		WriteMemoryException() :std::runtime_error(__func__) {}
		WriteMemoryException(const char *msg) :std::runtime_error(std::string(__func__) + " " + msg) {}
		WriteMemoryException(std::string msg) :std::runtime_error(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		读 A3 的内存失败。
	/// </summary>
	class ReadMemoryException :public std::runtime_error
	{
	public:
		ReadMemoryException() :std::runtime_error(__func__) {}
		ReadMemoryException(const char *msg) :std::runtime_error(std::string(__func__) + " " + msg) {}
		ReadMemoryException(std::string msg) :std::runtime_error(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		芯片状态异常。
	///		* 要求芯片处于运行状态，但是不是。
	///		* 要求芯片处于停止状态，但是不是。
	///		* 总之就是要求芯片处于某种状态或模式，但是不对。
	/// </summary>
	class ChipStatusException :public std::runtime_error
	{
	public:
		ChipStatusException() :std::runtime_error(__func__) {}
		ChipStatusException(const char *msg) :std::runtime_error(std::string(__func__) + " " + msg) {}
		ChipStatusException(std::string msg) :std::runtime_error(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		A3 向主机返回的数据是非法的或未定义的。
	/// </summary>
	class A3DataException :public std::runtime_error
	{
	public:
		A3DataException() :std::runtime_error(__func__) {}
		A3DataException(const char *msg) :std::runtime_error(std::string(__func__) + " " + msg) {}
		A3DataException(std::string msg) :std::runtime_error(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		USB 传输异常。
	/// </summary>
	class UsbException :public std::runtime_error
	{
	public:
		UsbException() :std::runtime_error(__func__) {}
		UsbException(const char *msg) :std::runtime_error(std::string(__func__) + " " + msg) {}
		UsbException(std::string msg) :std::runtime_error(std::string(__func__) + " " + msg) {}
	};
}
