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