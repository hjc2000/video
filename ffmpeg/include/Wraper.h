#pragma once
#include<iostream>
using std::cout;
using std::endl;
#include<functional>
using std::function;
#include<memory>
using std::shared_ptr;

/// <summary>
/// 这个类不定义纯虚函数。这个类不用来呈现多态，因为这个类用来给子类提供相同的功能，它并不是用来抽象
/// 子类的，只是提供一些相同的功能，减少编写代码中的重复劳动而已。
/// </summary>
/// <typeparam name="T">被包装对象的类型</typeparam>
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
	/// 
	Wraper(const Wraper &wraper)
	{
		// 复制共享指针，以此递增引用计数
		_refCount = wraper._refCount;
		// 将对方的指针复制过来
		_pWrapedObj = wraper._pWrapedObj;
	}

	virtual ~Wraper() {}

private:
	/// <summary>
	/// * 禁止使用一个已经构造完成的对象对另一个已经构造完成的对象赋值。如果不重载这个运算符并设为
	/// private，编译器会生成一个默认的，这会导致内存泄漏，因为被赋值的对象的 _pWrapedObj 失去
	/// 了原本指向的地址，无法对它进行释放了
	/// * 父类 Wraper 重载这个运算符并设为 private 后，所有子类，如果不自己实现一个一个 public 的，
	/// 都无法使用这个运算符了。注意，父类实现复制运算符并不是重写此运算符，因为这不是虚函数。而且前面也说了，
	/// 包装类不是用来实现多态的，只是提供一些相同的功能，减少编写代码中的重复劳动而已。
	/// </summary>
	/// <param name="ref"></param>
	/// <returns></returns>
	void operator=(const Wraper &ref) {}

protected:
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
	void operator=(T *pWrapedObj)
	{
		_pWrapedObj = pWrapedObj;
	}

	void operator=(T &ref_WrapedObj)
	{
		_pWrapedObj = &ref_WrapedObj;
	}

	T *operator()(void)
	{
		return _pWrapedObj;
	}

	operator T **()
	{
		return &_pWrapedObj;
	}

	operator T *()
	{
		return _pWrapedObj;
	}

	operator bool()
	{
		return _pWrapedObj;
	}

protected:
	T *_pWrapedObj = nullptr;
};