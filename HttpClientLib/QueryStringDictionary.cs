using JCNET.Parse;
using System.Collections;
using System.Diagnostics.CodeAnalysis;
using System.Text;

namespace HttpClientLib;
public class QueryStringDictionary : IDictionary<string, string>
{
	#region 生命周期
	public QueryStringDictionary() { }

	public QueryStringDictionary(Uri uri)
	{
		string[] query_strings = SeperateQeuryString(uri.Query);
		foreach (string query_string in query_strings)
		{
			ParseKeyValueResult? result = query_string.ParseKeyValueEx();
			if (result is not null)
			{
				Add(result.Value.Key, result.Value.Value);
			}
		}
	}
	#endregion

	/// <summary>
	/// 替换传入的 Uri 的 Query 属性
	/// </summary>
	/// <param name="uri"></param>
	/// <returns>返回一个 Query 属性被替换成本字典 ToString 结果的全新的 Uri</returns>
	public Uri ReplaceQuery(Uri uri)
	{
		UriBuilder builder = new(uri)
		{
			Query = ToString()
		};
		return builder.Uri;
	}

	private Dictionary<string, string> _query_dic = [];

	private static string[] SeperateQeuryString(string query_str)
	{
		if (query_str.StartsWith('?'))
		{
			query_str = query_str[1..];
		}

		return query_str.Split('&', StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries);
	}

	/// <summary>
	///		获得查询字符串。包括开头的 "?"。如果字典中没有储存任何查询参数，则返回空字符串。
	/// </summary>
	/// <returns></returns>
	public override string ToString()
	{
		if (_query_dic.Count == 0)
		{
			return string.Empty;
		}

		StringBuilder sb = new();
		sb.Append('?');
		foreach (KeyValuePair<string, string> pair in _query_dic)
		{
			if (sb.Length > 1)
			{
				/* 大于 1 表示除了开头的问号，后面还有内容，也就是至少有 1 个查询参数。因为后面还要添加新的
				 * 查询参数，所以要追加一个 & 字符。
				 */
				sb.Append('&');
			}

			// 追加查询参数
			sb.Append($"{pair.Key}={pair.Value}");
		}

		return sb.ToString();
	}

	public void Add(string key, string value)
	{
		_query_dic.Add(key, value);
	}

	public bool ContainsKey(string key)
	{
		return _query_dic.ContainsKey(key);
	}

	public bool Remove(string key)
	{
		return _query_dic.Remove(key);
	}

	public bool TryGetValue(string key, [MaybeNullWhen(false)] out string value)
	{
		return _query_dic.TryGetValue(key, out value);
	}

	public string this[string key]
	{
		get
		{
			return _query_dic[key];
		}
		set
		{
			_query_dic[key] = value;
		}
	}

	public ICollection<string> Keys
	{
		get
		{
			return _query_dic.Keys;
		}
	}

	public ICollection<string> Values
	{
		get
		{
			return _query_dic.Values;
		}
	}

	public void Add(KeyValuePair<string, string> item)
	{
		_query_dic.Add(item.Key, item.Value);
	}

	public void Clear()
	{
		_query_dic.Clear();
	}

	public bool Contains(KeyValuePair<string, string> item)
	{
		return _query_dic.Contains(item);
	}

	public void CopyTo(KeyValuePair<string, string>[] array, int arrayIndex)
	{
		int offset = 0;
		foreach (KeyValuePair<string, string> pair in _query_dic)
		{
			array[arrayIndex + offset++] = pair;
		}
	}

	public bool Remove(KeyValuePair<string, string> item)
	{
		return _query_dic.Remove(item.Key);
	}

	public int Count
	{
		get
		{
			return _query_dic.Count;
		}
	}

	public bool IsReadOnly
	{
		get
		{
			return false;
		}
	}

	public IEnumerator<KeyValuePair<string, string>> GetEnumerator()
	{
		return _query_dic.GetEnumerator();
	}

	IEnumerator IEnumerable.GetEnumerator()
	{
		return _query_dic.GetEnumerator();
	}
}
