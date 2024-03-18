using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.DependencyInjection;
using System.Net;
using System.Net.WebSockets;

namespace HttpServerLib.Websocket连接池;

public static class Websocket连接池中间件
{
	/*
		要求注入的服务：
		builder.Services.AddSingleton<WebsocketConnectionPool>();
		builder.Services.AddSingleton<IWebsocketUserIdParser, WebsocketUserIdParser>();
			
		WebsocketUserIdParser 只是个例子。用户需要自己创建一个类，继承 IWebsocketUserIdParser 接口，
		然后添加到服务中。
	 */
	/// <summary>
	///		使用 WebSocket 连接池中间件。会接收 WebSocket 连接，放到连接池中。
	///		<br/> 本中间件需要依赖注入。详细的看代码上方的注释。
	/// </summary>
	/// <param name="app"></param>
	/// <returns></returns>
	public static WebApplication UseWebsocketConnectionPool(this WebApplication app)
	{
		// 接收 accessor 连接，放到连接池中
		app.Map("/ws", async (context) =>
		{
			// 过滤非 accessor 请求
			if (!context.WebSockets.IsWebSocketRequest)
			{
				Console.WriteLine("收到的不是 websocket 请求。");
				context.Response.StatusCode = (int)HttpStatusCode.BadRequest;
				return;
			}

			// 取得 accessor 连接实例
			WebSocket websocket = await context.WebSockets.AcceptWebSocketAsync();
			WebsocketConnectionPool pool = app.Services.GetRequiredService<WebsocketConnectionPool>();

			// 准备 WebsocketAccessor
			IWebsocketUserIdParser user_id_parser = app.Services.GetRequiredService<IWebsocketUserIdParser>();
			WebsocketAccessor accessor = new(user_id_parser.ParseUserId(context), websocket);

			// 连接意外断开，从连接池中取出连接，并取消管道阻塞
			context.RequestAborted.Register(() =>
			{
				bool remove_result = pool.RevomeConnection(accessor);
				if (!remove_result)
				{
					Console.WriteLine("从连接池中移除 WebsocketAccessor 失败，意味着连接池中已没有此连接");
				}

				accessor.Discard();
			});

			// 将 accessor 连接放到集合中，然后等待使用完毕。
			pool.Put(accessor);
			await accessor.AccessingCompleted;

			// 清理连接
			Console.WriteLine("管道取消阻塞，准备清理连接");
			websocket.Dispose();
		});

		return app;
	}
}

/// <summary>
///		Websocket连接池中间件依赖的服务。需要在服务中注入。
/// 
///		收到 accessor 连接后，会把 HttpContext 传给这个接口，让它分析用户 ID 应该是什么，
///		并返回用户 ID。
/// </summary>
public interface IWebsocketUserIdParser
{
	string ParseUserId(HttpContext context);
}

