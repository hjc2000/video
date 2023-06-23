#pragma once
#include<iostream>
using std::cout;
using std::endl;
#include<functional>
using std::function;

template <class T>
class Wraper
{
public:
	Wraper() {}
	/// <summary>
	/// 将指针指向的对象交给包装类管理。包装类负责释放资源。不要私自对传进来的这个指针进行资源释放
	/// </summary>
	/// <param name="p"></param>
	Wraper(T* p)
	{
		_pWrapedObj = p;
	}
	/// <summary>
	/// 将引用指向的对象交给包装类管理。
	/// </summary>
	/// <param name="ref"></param>
	Wraper(T& ref)
	{
		_pWrapedObj = &ref;
	}
	/// <summary>
	/// 拷贝构造函数
	/// </summary>
	/// <param name="wraper"></param>
	Wraper(const Wraper& wraper)
	{
		// 通知源对象：你被拷贝了。如果发生了拷贝，则由副本负责释放资源
		*wraper._pCopyed = true;
		_pWrapedObj = wraper._pWrapedObj;
	}
	/// <summary>
	/// 用来标识本对象是否被拷贝了，如果是的话，由副本负责释放被包装的资源，自己撒手不管
	/// </summary>
	bool* _pCopyed = new bool{ false };
	/// <summary>
	/// 调用虚函数 DisposeWrapedObj 进行被包装对象的资源释放。虽然是虚析构函数，但即使子类实现了
	/// 析构函数，父类的析构函数也会执行。
	/// </summary>
	virtual ~Wraper()
	{
		// 如果发生拷贝，由副本负责释放资源，自己就撒手不管了
		if (!*_pCopyed)
		{
			DisposeWrapedObj();
		}

		delete _pCopyed;
	}
	/// <summary>
	/// 释放被包装的对象。子类不要在析构函数内部对资源进行释放，因为可能会对资源重复释放造成访问非法指针。
	/// 除非学 Wraper 的析构函数那样使用 _pCopyed 字段来避免对象拷贝后对一个指针进行重复释放资源
	/// </summary>
	virtual void DisposeWrapedObj() {}

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