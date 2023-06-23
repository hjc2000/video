#pragma once
#include<iostream>
using std::cout;
using std::endl;
#include<functional>
using std::function;
#include<memory>
using std::shared_ptr;

template <class T>
class Wraper
{
public:
	/// <summary>
	/// 无参构造函数
	/// </summary>
	Wraper() {}

	/// <summary>
	/// 拷贝构造函数
	/// </summary>
	/// <param name="wraper"></param>
	Wraper(const Wraper& wraper)
	{
		// 复制共享指针，以此递增引用计数
		_refCount = wraper._refCount;
		_pWrapedObj = wraper._pWrapedObj;
	}

	virtual ~Wraper() {}

	/// <summary>
	/// 利用它内部维护的引用计数
	/// </summary>
	shared_ptr<int> _refCount = shared_ptr<int>{ new int{0} };

	/// <summary>
	/// 让子类在析构函数中调用，从而知道自己是否应该释放资源
	/// </summary>
	/// <returns>
	/// 如果被包装的对象的引用计数为 1，即只剩下一个引用（说的就是你，正在执行析构函数的那个），
	/// 则返回 true ，子类应该在当前析构函数中释放资源
	/// </returns>
	bool should_dispose()
	{
		return _refCount.use_count() <= 1;
	}

public:
	void operator=(T* pWrapedObj)
	{
		_pWrapedObj = pWrapedObj;
	}

	void operator=(T& ref_WrapedObj)
	{
		_pWrapedObj = &ref_WrapedObj;
	}

	T* operator()(void)
	{
		return _pWrapedObj;
	}

	operator T** ()
	{
		return &_pWrapedObj;
	}

	operator T* ()
	{
		return _pWrapedObj;
	}

	operator bool()
	{
		return _pWrapedObj;
	}

protected:
	T* _pWrapedObj = nullptr;
};