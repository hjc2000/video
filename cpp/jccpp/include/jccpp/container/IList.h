#pragma once

template<typename T>
class IList
{
public:
	virtual ~IList() = default;
	virtual void Add(const T &item) = 0;
	virtual void Insert(int index, const T &item) = 0;
	virtual bool Remove(const T &item) = 0;
	virtual void RemoveAt(int index) = 0;
	virtual int IndexOf(const T &item) = 0;
	virtual bool Contains(const T &item) = 0;
	virtual void Clear() = 0;
	virtual int Count() const = 0;
	virtual T &operator[](int index) = 0;
	virtual T const &operator[](int index) const = 0;

	#pragma region 迭代器
	class IListIterator
	{
	private:
		IList<T> *_list;
		int _index;

	public:
		IListIterator(IList<T> *list, int index) :
			_list(list),
			_index(index)
		{

		}

		T &operator*() const
		{
			return (*_list)[_index];
		}

		IListIterator &operator++()
		{
			++_index;
			return *this;
		}

		bool operator!=(IListIterator const &other) const
		{
			return _index != other._index;
		}
	};

	IListIterator begin()
	{
		return IListIterator(this, 0);
	}

	IListIterator end()
	{
		return IListIterator(this, Count());
	}
	#pragma endregion

	/// <summary>
	///		两个 IList 对象的指针相等时才认为相等。
	/// </summary>
	/// <param name="another"></param>
	/// <returns></returns>
	bool operator==(IList const &another) const
	{
		return this == &another;
	}
};
