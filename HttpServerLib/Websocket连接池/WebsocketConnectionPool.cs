using System.Collections.Concurrent;

namespace HttpServerLib.Websocket连接池;
public class WebsocketConnectionPool
{
	private readonly ConcurrentDictionary<string, WebsocketCollection> _dic = new();

	public void Put(WebsocketAccessor accessor)
	{
		// 如果此用户没有 accessor 集合，则尝试为此用户添加一个 accessor 集合。
		_dic.TryAdd(accessor.UserId, new WebsocketCollection());

		/* 重新取出 WebsocketCollection。
		 * 禁止在本函数作用域内 new 一个 WebsocketCollection 后直接使用。
		 * 因为 new 出来的 WebsocketCollection 不一定放的进字典里，因为有可能有另一个
		 * 线程也在这么干，被抢先了。
		 */
		WebsocketCollection websocket_collection = _dic[accessor.UserId];
		websocket_collection.Add(accessor);
	}

	public WebsocketAccessor? TryTackOut(string user_id)
	{
		if (!_dic.TryGetValue(user_id, out WebsocketCollection? collection))
		{
			return null;
		}

		WebsocketAccessor? accessor = collection.TackOut();
		return accessor;
	}

	/// <summary>
	///		尝试取出连接。会尝试几次，每次间隔 1 秒。如果还是没有拿到，则返回 null.
	/// </summary>
	/// <param name="user_id"></param>
	/// <returns></returns>
	public async Task<WebsocketAccessor?> TryTackOutAsync(string user_id)
	{
		WebsocketAccessor? accessor = null;
		for (int i = 0; i < 5; i++)
		{
			accessor = TryTackOut("user_id");
			if (accessor is not null)
			{
				break;
			}

			await Task.Delay(1000);
		}

		return accessor;
	}

	/// <summary>
	///		移除指定的连接
	/// </summary>
	/// <param name="accessor"></param>
	/// <returns>移除成功返回 true，失败返回 false。</returns>
	public bool RevomeConnection(WebsocketAccessor accessor)
	{
		if (!_dic.TryGetValue(accessor.UserId, out WebsocketCollection? collection))
		{
			return false;
		}

		return collection.Remove(accessor);
	}
}