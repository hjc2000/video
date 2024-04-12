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
private:
	template<typename AnyType>
	friend bool operator==(Wrapper<AnyType> const &left, Wrapper<AnyType> const &right);

public:
	Wrapper() = default;
	virtual ~Wrapper() = default;

protected:
	T *_wrapped_obj = nullptr;

private:
	Wrapper(Wrapper const &ref) = delete;
	void operator=(Wrapper const &ref) = delete;

public:
	/// <summary>
	///		访问本类中储存的被包装类型对象的指针
	/// </summary>
	/// <returns></returns>
	T *operator->() const
	{
		return _wrapped_obj;
	}

	/// <summary>
	///		将本类对象强制转换为被包装类型的指针
	/// </summary>
	operator T *() const
	{
		return _wrapped_obj;
	}

	/// <summary>
	/// 将本类对象强制转换为被包装类型的指针的指针
	/// </summary>
	operator T **()
	{
		return &_wrapped_obj;
	}

	/// <summary>
	///		本类中储存的被包装类型对象的指针是否是空指针
	/// </summary>
	operator bool() const
	{
		return _wrapped_obj;
	}
};

/**
 * @brief 两个包装类的 _wrapped_obj 指针相等时认为两个包装类相等。
 * @tparam T
 * @param left
 * @param right
 * @return
*/
template<typename T>
bool operator==(Wrapper<T> const &left, Wrapper<T> const &right)
{
	return left._wrapped_obj == right._wrapped_obj;
}
