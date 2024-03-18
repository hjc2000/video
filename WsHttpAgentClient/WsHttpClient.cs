namespace WsHttpAgentClient;
/// <summary>
///		服务器将 http 请求发过来，代替服务器进行请求后，将响应送给服务器。
/// </summary>
public class WsHttpClient
{
	/// <summary>
	/// 
	/// </summary>
	/// <param name="user_id"></param>
	/// <param name="ws_http_server_uri">要连接的服务器的 URI。</param>
	public WsHttpClient(string user_id, Uri ws_http_server_uri)
	{
		_server_uri = ws_http_server_uri;
		_user_id = user_id;
		for (int i = 0; i < 4; i++)
		{
			AddListener();
		}
	}

	private readonly string _user_id;
	private readonly List<Task> _listen_task_list = [];
	private readonly Uri _server_uri;

	/// <summary>
	/// 创建本类后调用此函数即可开始服务
	/// </summary>
	/// <returns></returns>
	public async Task RunAsync()
	{
		while (true)
		{
			if (_listen_task_list.Count > 0)
			{
				Task completed_task = await Task.WhenAny(_listen_task_list);
				lock (_listen_task_list)
				{
					_listen_task_list.Remove(completed_task);
				}
			}

			// 连接数不足就添加连接
			while (_listen_task_list.Count < 4)
			{
				AddListener();
				Console.WriteLine("添加连接");
				await Task.Delay(1000);
			}
		}
	}

	private void AddListener()
	{
		lock (_listen_task_list)
		{
			if (_listen_task_list.Count < 4)
			{
				WsHttpRequestListener listener = new(_user_id, _server_uri);
				_listen_task_list.Add(listener.ListenAsync());
			}
		}
	}
}
