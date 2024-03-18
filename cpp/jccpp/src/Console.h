#pragma once
#include<iostream>
#include<mutex>

using namespace std;

/// <summary>
///		线程安全的控制台输出类。使用了互斥量，保证在多线程中输出时，不同线程输出
///		的内容不会互相干扰或覆盖，从而保证输出内容的完整性。
/// </summary>
class Console
{
	mutex &lock()
	{
		static mutex m;
		return m;
	}

public:
	void WriteLine()
	{
		lock_guard l(lock());
		cout << endl;
	}

	template<typename T>
	void WriteLine(T value)
	{
		lock_guard l(lock());
		cout << value << endl;
	}

	template<typename T>
	void Write(T value)
	{
		lock_guard l(lock());
		cout << value;
	}

	// For general types
	template<typename T>
	Console &operator<<(const T &value)
	{
		lock_guard<mutex> l(lock());
		cout << value;
		return *this;
	}

	// Specialized for std::endl and similar manipulators
	Console &operator<<(ostream &(*manipulator)(ostream &))
	{
		lock_guard<mutex> l(lock());
		cout << manipulator;
		return *this;
	}
};
