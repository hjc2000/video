#pragma once

template<typename T>
class IList
{
public:
	virtual ~IList() = default;

	// 添加元素到列表的末尾
	virtual void Add(const T &item) = 0;

	void Add(IList<T> &another)
	{
		for (int i = 0; i < another.Count(); i++)
		{
			Add(another[i]);
		}
	}

	// 在指定索引处插入一个元素
	virtual void Insert(int index, const T &item) = 0;

	// 删除与给定对象相等的第一个元素
	virtual bool Remove(const T &item) = 0;

	// 移除索引为 index 的元素
	virtual void RemoveAt(int index) = 0;

	// 返回与给定对象相等的第一个元素的索引
	virtual int IndexOf(const T &item) = 0;

	// 检查列表中是否包含给定的元素
	virtual bool Contains(const T &item) = 0;

	// 清除列表中的所有元素
	virtual void Clear() = 0;

	// 获取列表的大小
	virtual int Count() const = 0;

	// 通过索引访问元素
	virtual T &operator[](int index) = 0;

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
