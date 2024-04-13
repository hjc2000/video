#pragma once
#include<iostream>
using std::cout;
using std::endl;
#include<functional>
using std::function;
#include<memory>
using std::shared_ptr;

template <class T>
class Wrapper
{
	Wrapper(Wrapper const &ref) = delete;
	void operator=(Wrapper const &ref) = delete;

protected:
	T *_wrapped_obj = nullptr;

public:
	Wrapper() = default;
	virtual ~Wrapper() = default;

	virtual T *&WrappedObj()
	{
		return _wrapped_obj;
	}

	virtual T *WrappedObj() const
	{
		return _wrapped_obj;
	}

	/// <summary>
	///		访问本类中储存的被包装类型对象的指针
	/// </summary>
	/// <returns></returns>
	T *operator->() const
	{
		return WrappedObj();
	}

	/// <summary>
	///		将本类对象强制转换为被包装类型的指针
	/// </summary>
	operator T *() const
	{
		return WrappedObj();
	}

	/// <summary>
	/// 将本类对象强制转换为被包装类型的指针的指针
	/// </summary>
	operator T **()
	{
		return &WrappedObj();
	}

	/// <summary>
	///		本类中储存的被包装类型对象的指针是否是空指针
	/// </summary>
	operator bool() const
	{
		return WrappedObj();
	}

	bool operator==(Wrapper<T> const &another) const
	{
		return WrappedObj() == another.WrappedObj();
	}
};
