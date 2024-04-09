#pragma once

template<typename T>
class IList
{
public:
	virtual ~IList() {}

public:
	// 添加元素到列表的末尾
	virtual void Add(const T &item) = 0;

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
	virtual int Count() = 0;

	// 通过索引访问元素
	virtual T &operator[](int index) = 0;
};

/**
 * @brief 只有两个 IList 接口对象的指针相等的时候才认为对象相等。
 * @tparam T
 * @param list1
 * @param list2
 * @return
*/
template<typename T>
bool operator==(IList<T> const &list1, IList<T> const &list2)
{
	return &list1 == &list2;
}