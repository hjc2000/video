#pragma once
#include<exception>
#include<iostream>
#include<stdexcept>
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
	///		使用已经 Dispose 的对象会引发此异常。
	/// </summary>
	class ObjectDisposedException :public Exception
	{
	public:
		ObjectDisposedException() :Exception(__func__) {}
		ObjectDisposedException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		ObjectDisposedException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};
}
