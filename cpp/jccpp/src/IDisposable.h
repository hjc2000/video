#pragma once
#include<atomic>

class IDisposable
{
public:
	virtual ~IDisposable() {}

	/// <summary>
	///		C++ 不像 C# 那样有垃圾回收器。C++ 的析构函数在离开作用域后会自动调用。
	///		本接口是为了面对一些在析构之前有任务必须做的场景。
	/// 
	///		例如：
	///		* 一个类创建了一个线程，在析构的时候需要让线程停下来。
	///		* 一个类内部有信号量，并且有方法会被信号量阻塞。在析构的时候必须让被阻塞的
	///		  方法结束阻塞。
	/// </summary>
	virtual void Dispose() = 0;
};