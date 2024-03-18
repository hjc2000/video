using System.Net.WebSockets;

namespace HttpServerLib.Websocket连接池;
/// <summary>
///		用于访问连接池中的 websocket。
/// </summary>
public class WebsocketAccessor(string user_id, WebSocket socket)
{
	public WebSocket Socket { get; } = socket;
	public string UserId { get; } = user_id;

	/// <summary>
	///		连接使用完毕。管道已经取消阻塞了，将会清理连接。
	/// </summary>
	public Task AccessingCompleted
	{
		get
		{
			return _accessing_task_completion_source.Task;
		}
	}

	private readonly TaskCompletionSource _accessing_task_completion_source = new();
	/// <summary>
	///		将连接丢弃。
	///		<br/> 连接会回到管道。会取消管道的阻塞，随后管道会清理连接。
	/// </summary>
	public void Discard()
	{
		_accessing_task_completion_source.TrySetResult();
	}
}
