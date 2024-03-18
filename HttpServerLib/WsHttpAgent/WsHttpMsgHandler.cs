using HttpServerLib.HTTP请求响应头部信息;
using System.Net.WebSockets;
using System.Text.Json;

namespace HttpServerLib.WsHttpAgent;

public class WsHttpMsgHandler : HttpMessageHandler
{
	public WsHttpMsgHandler(WebSocket socket)
	{
		_websocket_stream = new WebSocketStream(socket);
	}

	private readonly WebSocketStream _websocket_stream;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="request"></param>
	/// <param name="cancellationToken"></param>
	/// <returns></returns>
	/// <exception cref="HttpRequestException"></exception>
	protected override async Task<HttpResponseMessage> SendAsync(HttpRequestMessage request, CancellationToken cancellationToken)
	{
		// 发送请求头
		HttpRequestHead request_head = new(request);
		await request_head.SerializeToJsonStreamAsync(_websocket_stream, cancellationToken);
		await _websocket_stream.FlushAsync(cancellationToken);

		// 发送请求体（如果请求体不为空的话）
		if (request.Content is not null)
		{
			Stream request_content = await request.Content.ReadAsStreamAsync(cancellationToken);
			await request_content.CopyToAsync(_websocket_stream, cancellationToken);
			await _websocket_stream.FlushAsync(cancellationToken);
		}

		// 接收响应信息
		await _websocket_stream.SyncReadingFrame(cancellationToken);
		HttpResponseHead? response_infos = await JsonSerializer.DeserializeAsync<HttpResponseHead>(
			_websocket_stream,
			(JsonSerializerOptions?)null,
			cancellationToken
		);

		if (response_infos is null)
		{
			throw new HttpRequestException("接收到的响应信息为空", null, System.Net.HttpStatusCode.NotFound);
		}

		// 接收响应体
		await _websocket_stream.SyncReadingFrame();
		HttpResponseMessage response_msg = response_infos.CreateHttpResponseMessage(_websocket_stream);
		return response_msg;
	}
}
