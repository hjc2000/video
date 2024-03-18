namespace HttpServerLib.Websocket连接池;
/// <summary>
///		线程安全的 accessor 连接对象集合。
/// </summary>
public class WebsocketCollection
{
	private readonly LinkedList<WebsocketAccessor> _link_list = new();
	private readonly object _lock = new();

	/// <summary>
	///		取出集合中的 WebSocket 对象。如果集合为空，返回 null。
	/// </summary>
	/// <returns>WebSocket 对象。如果集合为空，返回 null。</returns>
	public WebsocketAccessor? TackOut()
	{
		lock (_lock)
		{
			if (_link_list.Count == 0)
			{
				return null;
			}

			WebsocketAccessor tacked = _link_list.First();
			_link_list.RemoveFirst();
			return tacked;
		}
	}

	public void Add(WebsocketAccessor item)
	{
		lock (_lock)
		{
			_link_list.AddLast(item);
		}
	}

	public void Clear()
	{
		lock (_lock)
		{
			_link_list.Clear();
		}
	}

	public bool Contains(WebsocketAccessor item)
	{
		lock (_lock)
		{
			return _link_list.Contains(item);
		}
	}

	public bool Remove(WebsocketAccessor item)
	{
		lock (_lock)
		{
			return _link_list.Remove(item);
		}
	}

	public int Count
	{
		get
		{
			lock (_lock)
			{
				return _link_list.Count;
			}
		}
	}
}
