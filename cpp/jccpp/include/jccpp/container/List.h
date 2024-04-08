#pragma once
#include<jccpp/Exception.h>
#include<jccpp/container/IList.h>
#include<vector>

template<typename T>
class List :public IList<T>
{
public:
	List() {}

	List(List const &another)
	{
		_vector = another._vector;
	}

	List(std::vector<T> const &vec)
	{
		_vector = vec;
	}

	List &operator=(List const &value)
	{
		_vector = value._vector;
		return *this;
	}

	List &operator=(std::vector<T> const &value)
	{
		_vector = value;
		return *this;
	}

private:
	std::vector<T> _vector;

public:
	// 添加元素到列表的末尾
	void Add(T const &item) override
	{
		_vector.push_back(item);
	}

	// 在指定索引处插入一个元素
	void Insert(int const index, T const &item) override
	{
		if (index<0 || index > _vector.size())
		{
			throw jc::OutOfRangeException();
		}

		_vector.insert(_vector.begin() + index, item);
	}

	// 删除与给定对象相等的第一个元素
	bool Remove(T const &item) override
	{
		auto it = std::find(_vector.begin(), _vector.end(), item);
		if (it != _vector.end())
		{
			_vector.erase(it);
			return true; // 返回 true 表示成功找到并删除了元素
		}

		return false; // 如果没有找到元素，返回 false
	}

	// 移除索引为 index 的元素
	void RemoveAt(int const index) override
	{
		if (index < 0 || index >= _vector.size())
		{
			throw jc::OutOfRangeException();
		}

		_vector.erase(_vector.begin() + index);
	}

	// 返回与给定对象相等的第一个元素的索引
	int IndexOf(T const &item) override
	{
		auto it = std::find(_vector.begin(), _vector.end(), item);
		if (it != _vector.end())
		{
			return (int)std::distance(_vector.begin(), it); // 返回元素的索引
		}

		return -1; // 如果没有找到元素，返回 -1
	}

	// 检查列表中是否包含给定的元素
	bool Contains(T const &item) override
	{
		return std::find(_vector.begin(), _vector.end(), item) != _vector.end();
	}

	// 清除列表中的所有元素
	void Clear() override
	{
		_vector.clear();
	}

	// 获取列表的大小
	int Count() override
	{
		return (int)_vector.size();
	}

	// 通过索引访问元素
	T &operator[](int const index) override
	{
		if (index < 0 || index >= _vector.size())
		{
			throw jc::OutOfRangeException();
		}

		return _vector[index];
	}

	/// <summary>
	///		强制转换为向量。
	/// </summary>
	operator std::vector<T>()
	{
		return _vector;
	}

	#pragma region 迭代器
	// 提供迭代器的类型定义
	using iterator = typename std::vector<T>::iterator;

	// 提供开始迭代器
	iterator begin()
	{
		return _vector.begin();
	}

	// 提供结束迭代器
	iterator end()
	{
		return _vector.end();
	}
	#pragma endregion
};
