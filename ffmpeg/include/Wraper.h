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
	#pragma region 生命周期
public:
	/// <summary>
	/// 无参构造函数
	/// </summary>
	Wraper() {}

	/// <summary>
	/// 通过被包装对象的指针进行构造
	/// </summary>
	/// <param name="p"></param>
	Wraper(const T *p)
	{
		w = (T *)p;
	}

	/// <summary>
	/// 通过被包装对象的引用进行构造
	/// </summary>
	/// <param name="ref"></param>
	Wraper(const T &ref)
	{
		w = (T *)&ref;
	}


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
		w = wraper.w;
	}

	virtual ~Wraper() {}

	/// <summary>
	/// 子类如果需要释放资源，一定重写此方法以供父类在赋值运算符中调用
	/// </summary>
	virtual void Dispose() {}
	#pragma endregion

	/// <summary>
	/// 被包装的对象的指针
	/// </summary>
	T *w = nullptr;

	#pragma region 运算符重载
public:
	/// <summary>
	/// 重载赋值运算符
	/// </summary>
	/// <param name="ref"></param>
	void operator=(const Wraper &ref)
	{
		// 防止自赋值
		if (this == &ref) return;
		// 如果本对象已经分配 w 资源了就要先释放
		if (w)
			Dispose();
		// 将对方的资源拿过来
		w = ref.w;
		// 递增引用计数，只要是复制 _pWrapedObj，必须同时复制 _refCount
		_refCount = ref._refCount;
	}

	void operator=(T *pWrapedObj)
	{
		w = pWrapedObj;
	}

	void operator=(T &ref_WrapedObj)
	{
		w = &ref_WrapedObj;
	}

	T *operator()(void)
	{
		return w;
	}

	operator T **()
	{
		return &w;
	}

	operator T *()
	{
		return w;
	}

	operator bool()
	{
		return w;
	}
	#pragma endregion

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
};