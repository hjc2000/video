using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Primitives;
using System.Collections;
using System.Diagnostics.CodeAnalysis;
using System.Net.Http.Headers;
using System.Text;

namespace HttpServerLib.HTTP请求响应头部信息;

/// <summary>
///		用标准库的普通的，支持序列化，反序列化的容器来储存标头信息
/// </summary>
public class HttpHeaderDictionary : IDictionary<string, List<string>>
{
	public HttpHeaderDictionary() { }

	public HttpHeaderDictionary(IHeaderDictionary header_dic)
	{
		foreach (KeyValuePair<string, StringValues> pair in header_dic)
		{
			string key = pair.Key;
			List<string> value_list = [];
			foreach (string? value in pair.Value)
			{
				if (value is not null)
				{
					value_list.Add(value);
				}
			}

			_dic[key] = value_list;
		}
	}

	public HttpHeaderDictionary(HttpHeaders header_dic)
	{
		foreach (KeyValuePair<string, IEnumerable<string>> pair in header_dic)
		{
			string key = pair.Key;
			List<string> value_list = [];
			foreach (string? value in pair.Value)
			{
				if (value is not null)
				{
					value_list.Add(value);
				}
			}

			_dic[key] = value_list;
		}
	}

	private readonly Dictionary<string, List<string>> _dic = [];

	public void Add(string key, List<string> value)
	{
		((IDictionary<string, List<string>>)_dic).Add(key, value);
	}

	public bool ContainsKey(string key)
	{
		return ((IDictionary<string, List<string>>)_dic).ContainsKey(key);
	}

	public bool Remove(string key)
	{
		return ((IDictionary<string, List<string>>)_dic).Remove(key);
	}

	public bool TryGetValue(string key, [MaybeNullWhen(false)] out List<string> value)
	{
		return ((IDictionary<string, List<string>>)_dic).TryGetValue(key, out value);
	}

	public List<string> this[string key]
	{
		get
		{
			return ((IDictionary<string, List<string>>)_dic)[key];
		}

		set
		{
			((IDictionary<string, List<string>>)_dic)[key] = value;
		}
	}

	public ICollection<string> Keys
	{
		get
		{
			return ((IDictionary<string, List<string>>)_dic).Keys;
		}
	}

	public ICollection<List<string>> Values
	{
		get
		{
			return ((IDictionary<string, List<string>>)_dic).Values;
		}
	}

	public void Add(KeyValuePair<string, List<string>> item)
	{
		((ICollection<KeyValuePair<string, List<string>>>)_dic).Add(item);
	}

	public void Clear()
	{
		((ICollection<KeyValuePair<string, List<string>>>)_dic).Clear();
	}

	public bool Contains(KeyValuePair<string, List<string>> item)
	{
		return ((ICollection<KeyValuePair<string, List<string>>>)_dic).Contains(item);
	}

	public void CopyTo(KeyValuePair<string, List<string>>[] array, int arrayIndex)
	{
		((ICollection<KeyValuePair<string, List<string>>>)_dic).CopyTo(array, arrayIndex);
	}

	public bool Remove(KeyValuePair<string, List<string>> item)
	{
		return ((ICollection<KeyValuePair<string, List<string>>>)_dic).Remove(item);
	}

	public int Count
	{
		get
		{
			return ((ICollection<KeyValuePair<string, List<string>>>)_dic).Count;
		}
	}

	public bool IsReadOnly
	{
		get
		{
			return ((ICollection<KeyValuePair<string, List<string>>>)_dic).IsReadOnly;
		}
	}

	public IEnumerator<KeyValuePair<string, List<string>>> GetEnumerator()
	{
		return ((IEnumerable<KeyValuePair<string, List<string>>>)_dic).GetEnumerator();
	}

	IEnumerator IEnumerable.GetEnumerator()
	{
		return ((IEnumerable)_dic).GetEnumerator();
	}

	/// <summary>
	///		按照 http 请求头的格式，将本字典中的信息变成字符串。
	/// </summary>
	/// <returns></returns>
	public override string ToString()
	{
		StringBuilder sb = new();
		foreach (KeyValuePair<string, List<string>> pair in _dic)
		{
			sb.Append($"{pair.Key}: ");
			sb.AppendLine(string.Join(", ", pair.Value));
		}

		return sb.ToString().TrimEnd();
	}
}
