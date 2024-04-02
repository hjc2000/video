#pragma once
#include<exception>
#include<iostream>
#include<string>

std::ostream &operator<<(std::ostream &os, std::exception const &e);

namespace jc
{
	class Exception : public std::exception
	{
	public:
		Exception() {}

		Exception(const char *msg)
		{
			_msg = std::string{ msg };
		}

		Exception(std::string msg)
		{
			_msg = msg;
		}

		~Exception() {}

	public:
		const char *what() const noexcept override
		{
			return _msg.c_str();
		}

	private:
		std::string _msg;
	};

	/// <summary>
	///		此异常表示：开发中，此功能没有完善。
	/// </summary>
	class NotImplementedException :public Exception
	{
	public:
		NotImplementedException() :Exception(__func__) {}
		NotImplementedException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		NotImplementedException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	class NotSupportedException :public Exception
	{
	public:
		NotSupportedException() :Exception(__func__) {}
		NotSupportedException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		NotSupportedException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		不允许为空指针的地方出现了空指针。
	/// </summary>
	class ArgumentNullException :public Exception
	{
	public:
		ArgumentNullException() :Exception(__func__) {}
		ArgumentNullException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		ArgumentNullException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		非法操作
	/// </summary>
	class InvalidOperationException :public Exception
	{
	public:
		InvalidOperationException() :Exception(__func__) {}
		InvalidOperationException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		InvalidOperationException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		参数非法。
	/// </summary>
	class ArgumentException :public Exception
	{
	public:
		ArgumentException() :Exception(__func__) {}
		ArgumentException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		ArgumentException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		使用已经 Dispose 的对象会引发此异常。
	/// </summary>
	class ObjectDisposedException :public Exception
	{
	public:
		ObjectDisposedException() :Exception(__func__) {}
		ObjectDisposedException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		ObjectDisposedException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	/// <summary>
	///		缓冲区写入时发生上溢。
	/// </summary>
	class BufferOverflowException :public Exception
	{
	public:
		BufferOverflowException() :Exception(__func__) {}
		BufferOverflowException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		BufferOverflowException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	class OutOfRangeException :public Exception
	{
	public:
		OutOfRangeException() :Exception(__func__) {}
		OutOfRangeException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		OutOfRangeException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};
}
