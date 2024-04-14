#pragma once
#include<algorithm>
#include<jccpp/Exception.h>
#include<jccpp/container/IList.h>
#include<vector>

template<typename T>
class List :public IList<T>
{
private:
	std::vector<T> _vector;

public:
	List() = default;

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

	void Add(T const &item) override
	{
		_vector.push_back(item);
	}

	void Add(List<T> const &another)
	{
		for (int i = 0; i < another.Count(); i++)
		{
			Add(another[i]);
		}
	}

	void Insert(int const index, T const &item) override
	{
		if (index<0 || index > _vector.size())
		{
			throw jc::OutOfRangeException();
		}

		_vector.insert(_vector.begin() + index, item);
	}

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

	void RemoveAt(int const index) override
	{
		if (index < 0 || index >= _vector.size())
		{
			throw jc::OutOfRangeException();
		}

		_vector.erase(_vector.begin() + index);
	}

	int IndexOf(T const &item) override
	{
		auto it = std::find(_vector.begin(), _vector.end(), item);
		if (it != _vector.end())
		{
			return (int)std::distance(_vector.begin(), it); // 返回元素的索引
		}

		return -1; // 如果没有找到元素，返回 -1
	}

	bool Contains(T const &item) override
	{
		return std::find(_vector.begin(), _vector.end(), item) != _vector.end();
	}

	void Clear() override
	{
		_vector.clear();
	}

	int Count() const override
	{
		return (int)_vector.size();
	}

	T &operator[](int const index) override
	{
		if (index < 0 || index >= _vector.size())
		{
			throw jc::OutOfRangeException();
		}

		return _vector[index];
	}

	T const &operator[](int const index) const override
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

	/// <summary>
	///		转发到 std::vector 的相等判断逻辑。
	/// </summary>
	/// <param name="another"></param>
	/// <returns></returns>
	bool operator==(List const &another) const
	{
		return _vector == another._vector;
	}
};
