using HttpServerLib.HTTP请求响应头部信息;
using HttpServerLib.Websocket连接池;
using HttpServerLib.扩展方法;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.DependencyInjection;
using System.Net;

namespace HttpServerLib.WsHttpAgent;
public static class WsHttpAgent中间件
{
	/// <summary>
	///		将 http 请求转发给 websocket 代理的中间件。websocket 代理帮忙请求后返回响应报文。
	/// </summary>
	/// <param name="app"></param>
	/// <param name="routing_path"></param>
	/// <param name="agent_dst_base_uri"></param>
	/// <returns></returns>
	public static WebApplication UseWsHttpAgent(this WebApplication app, string routing_path, Uri agent_dst_base_uri)
	{
		app.Map(routing_path, async (HttpContext context) =>
		{
			HttpRequestMessage request_msg = await context.Request.ToHttpRequestMessage(agent_dst_base_uri);
			HttpRequestHead request_head = new(request_msg);
			Console.WriteLine(request_head.ToJsonString());

			WebsocketConnectionPool pool = app.Services.GetRequiredService<WebsocketConnectionPool>();
			WebsocketAccessor? accessor = await pool.TryTackOutAsync("user_id");
			if (accessor is null)
			{
				context.Response.StatusCode = (int)HttpStatusCode.GatewayTimeout;
				return;
			}

			// 发送请求
			try
			{
				HttpClient client = new(new WsHttpMsgHandler(accessor.Socket));
				HttpResponseMessage response_msg = await client.SendAsync(
					request_msg,
					HttpCompletionOption.ResponseHeadersRead
				);

				await context.Response.SendHttpResponseMessageAsync(
					response_msg, 
					context.RequestAborted
				);

				// 将连接放回连接池
				pool.Put(accessor);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.ToString());
				accessor.Discard();
			}
		});

		return app;
	}
}
