using System.Collections;

namespace JCNET.容器;
/// <summary>
///		* 迭代不是线程安全的。迭代需要手动用 lock 语句锁住本对象，防止迭代期间集合发生变化。
///		如果迭代期间需要 await，手动用 ToArray 方法复制一个快照，对着快照迭代。<br/>
///		
///		* 迭代本对象时，获取迭代器时会检测是否锁定本对象，如果发现未锁定，会抛出异常。
/// </summary>
/// <typeparam name="T"></typeparam>
public class SafeList<T> : IList<T>
{
	private List<T> _list = [];

	public int IndexOf(T item)
	{
		lock (this)
		{
			return ((IList<T>)_list).IndexOf(item);
		}
	}

	public void Insert(int index, T item)
	{
		lock (this)
		{
			((IList<T>)_list).Insert(index, item);
		}
	}

	public void RemoveAt(int index)
	{
		lock (this)
		{
			((IList<T>)_list).RemoveAt(index);
		}
	}

	public T this[int index]
	{
		get
		{
			lock (this)
			{
				return ((IList<T>)_list)[index];
			}
		}
		set
		{
			lock (this)
			{
				((IList<T>)_list)[index] = value;
			}
		}
	}

	public void Add(T item)
	{
		lock (this)
		{
			((ICollection<T>)_list).Add(item);
		}
	}

	public void Clear()
	{
		lock (this)
		{
			((ICollection<T>)_list).Clear();
		}
	}

	public bool Contains(T item)
	{
		lock (this)
		{
			return ((ICollection<T>)_list).Contains(item);
		}
	}

	public void CopyTo(T[] array, int arrayIndex)
	{
		lock (this)
		{
			((ICollection<T>)_list).CopyTo(array, arrayIndex);
		}
	}

	public T[] ToArray()
	{
		lock (this)
		{
			T[] arr = [.. _list];
			return arr;
		}
	}

	public bool Remove(T item)
	{
		lock (this)
		{
			return ((ICollection<T>)_list).Remove(item);
		}
	}

	public int Count
	{
		get
		{
			lock (this)
			{
				return ((ICollection<T>)_list).Count;
			}
		}
	}

	public bool IsReadOnly
	{
		get
		{
			return false;
		}
	}

	public IEnumerator<T> GetEnumerator()
	{
		if (!Monitor.IsEntered(this))
		{
			throw new InvalidOperationException("使用迭代器要手动锁定本对象");
		}

		return ((IEnumerable<T>)_list).GetEnumerator();
	}

	IEnumerator IEnumerable.GetEnumerator()
	{
		return GetEnumerator();
	}
}
