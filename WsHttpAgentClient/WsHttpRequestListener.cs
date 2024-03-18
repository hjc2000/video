using HttpServerLib;
using HttpServerLib.HTTP请求响应头部信息;
using System.Net.WebSockets;
using System.Text.Json;

namespace WsHttpAgentClient;

/// <summary>
///		监听一个 websocket 连接，收到服务器发过来的 http 请求后处理请求。
/// </summary>
internal class WsHttpRequestListener
{
	public WsHttpRequestListener(string user_id, Uri server_uri)
	{
		_user_id = user_id;
		_server_uri = server_uri;
		_socket.Options.SetRequestHeader("user_id", user_id);
		_wstream = new WebSocketStream(_socket);
	}

	private readonly string _user_id;
	private readonly Uri _server_uri;
	private readonly HttpClient _http_client = new();
	private readonly ClientWebSocket _socket = new();
	private readonly WebSocketStream _wstream;

	/// <summary>
	///		连接服务器并开始监听来自服务器的请求。
	/// </summary>
	/// <returns></returns>
	public async Task ListenAsync()
	{
		await _socket.ConnectAsync(_server_uri, CancellationToken.None);
		Console.WriteLine(
			$"{nameof(WsHttpRequestListener)}，" +
			$"{nameof(_user_id)} = {_user_id}；" +
			$"{nameof(_server_uri)} = {_server_uri} " +
			"连接服务器成功，开始监听来自服务器的请求。"
		);

		// 在死循环中监听
		while (true)
		{
			try
			{
				// 接受请求头
				await _wstream.SyncReadingFrame();
				HttpRequestHead? request_head = await JsonSerializer.DeserializeAsync<HttpRequestHead>(_wstream);
				if (request_head is null)
				{
					continue;
				}

				Console.WriteLine($"收到请求消息，请求的 URI：{request_head.RequestUri}");

				// 接收请求体
				if (request_head.ContentHeaders is not null)
				{
					// 有请求体才要进行帧同步，否则会把有用信息清理掉。
					await _wstream.SyncReadingFrame();
				}

				HttpRequestMessage request_msg = request_head.CreateHttpRequestMessage(_wstream);

				// 代替服务器发送请求
				HttpResponseMessage response_msg = await _http_client.SendAsync(request_msg, HttpCompletionOption.ResponseHeadersRead);
				Console.WriteLine("请求已发送");
				Console.WriteLine($"状态码：{response_msg.StatusCode}");

				// 返回请求头部
				HttpResponseHead response_head = new(response_msg);
				await JsonSerializer.SerializeAsync(_wstream, response_head);
				await _wstream.FlushAsync();

				// 返回请求体
				Stream response_stream = await response_msg.Content.ReadAsStreamAsync();
				await response_stream.CopyToAsync(_wstream);
				await _wstream.FlushAsync();
			}
			catch (JsonException e)
			{
				Console.WriteLine("json 反序列化发生异常");
				Console.WriteLine(e.ToString());
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
				break;
			}
		}

		Console.WriteLine("结束监听");
	}
}
