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

public:
	Wrapper() = default;
	virtual ~Wrapper() = default;

	/// <summary>
	///		被包装对象一般应该是通过指针指向的位于堆中的对象，因为这样允许运行时更换被包装对象。
	///		本函数返回的是指针的引用，允许修改指针的指向。派生类如果不允许指针被修改记得将本函数
	///		实现为抛出异常 jc::NotSupportedException()。
	/// </summary>
	/// <returns></returns>
	virtual T *&WrappedObj() = 0;
	virtual T *WrappedObj() const = 0;

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
	///		将本类对象强制转换为被包装类型的指针的指针。
	///		本函数返回的是指针的指针，也就是可以用来将被包装对象替换成另一个对象。
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
