using System.Collections;

namespace JCNET;
public class History<T> : IEnumerable<T> where T : class
{
	public History(int maxSize)
	{
		if (maxSize <= 0)
		{
			throw new ArgumentException("Max size must be greater than 0.", nameof(maxSize));
		}

		_record_list = [];
		_max_size = maxSize;
	}

	private readonly List<T> _record_list;
	private readonly int _max_size;

	/// <summary>
	///		添加一条记录。如果记录数量达到上限，会移除最旧的记录。
	/// </summary>
	/// <param name="record">如果 record 和最后一条记录相同，则不会重复添加。</param>
	public void Add(T record)
	{
		if (_record_list.Count > 0 && _record_list[^1] == record)
		{
			// 不重复放置
			return;
		}

		if (_record_list.Count >= _max_size)
		{
			_record_list.RemoveAt(0);
		}

		_record_list.Add(record);
	}

	/// <summary>
	///		获取最后一条记录。
	/// </summary>
	/// <returns>如果没有记录，返回 null。</returns>
	public T? GetLast()
	{
		if (_record_list.Count == 0)
		{
			return null;
		}

		T? lastRecord = _record_list[^1];
		_record_list.RemoveAt(_record_list.Count - 1);
		return lastRecord;
	}

	public bool Remove(T record)
	{
		return _record_list.Remove(record);
	}

	public void RemoveAt(int index)
	{
		_record_list.RemoveAt(index);
	}

	/// <summary>
	///		记录的数量。
	/// </summary>
	public int Count
	{
		get
		{
			return _record_list.Count;
		}
	}

	public IEnumerator<T> GetEnumerator()
	{
		return ((IEnumerable<T>)_record_list).GetEnumerator();
	}

	IEnumerator IEnumerable.GetEnumerator()
	{
		return ((IEnumerable)_record_list).GetEnumerator();
	}
}
