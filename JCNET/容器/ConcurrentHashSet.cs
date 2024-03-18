using System.Collections;
using System.Collections.Concurrent;

namespace JCNET.容器;
/// <summary>
///		线程安全的哈希表。基于 ConcurrentDictionary，缩水使用。键旧当作哈希表的键，值储存 null。
/// </summary>
/// <typeparam name="T"></typeparam>
public class ConcurrentHashSet<T> : ICollection<T> where T : class
{
	private ConcurrentDictionary<T, object?> _dic = [];

	public void Add(T item)
	{
		_dic[item] = null;
	}

	public void Clear()
	{
		_dic.Clear();
	}

	public bool Contains(T item)
	{
		return _dic.ContainsKey(item);
	}

	public void CopyTo(T[] array, int arrayIndex)
	{
		_dic.Keys.CopyTo(array, arrayIndex);
	}

	public bool Remove(T item)
	{
		return _dic.Remove(item, out _);
	}

	public int Count
	{
		get
		{
			return _dic.Count;
		}
	}

	public bool IsReadOnly
	{
		get
		{
			return false;
		}
	}

	/// <summary>
	///		可以安全地迭代。添加或删除不会破坏迭代器。但是，这并不是对集合拍一个快照后再迭代。
	///		迭代开始后，添加和删除会影响到迭代。例如还没被迭代到的地方多了一个元素，则等会迭代器
	///		会迭代到它。
	/// </summary>
	/// <returns></returns>
	public IEnumerator<T> GetEnumerator()
	{
		return _dic.Keys.GetEnumerator();
	}

	/// <summary>
	///		可以安全地迭代。添加或删除不会破坏迭代器。但是，这并不是对集合拍一个快照后再迭代。
	///		迭代开始后，添加和删除会影响到迭代。例如还没被迭代到的地方多了一个元素，则等会迭代器
	///		会迭代到它。
	/// </summary>
	/// <returns></returns>
	IEnumerator IEnumerable.GetEnumerator()
	{
		return _dic.Keys.GetEnumerator();
	}
}