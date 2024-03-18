using HttpClientLib.扩展方法;
using HttpServerLib.扩展方法;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.DependencyInjection;

namespace HttpServerLib.中间件;
public static class 普通正向代理
{
	/// <summary>
	///		使用正向代理中间件。转发请求使用的是普通的 HttpClient 类。
	/// </summary>
	/// <param name="app"></param>
	/// <param name="routing_path">
	///		路由的路径。
	///		本中间件基于 Map 中间件。这里就是要路由的路径。只有在此路径的请求会被代理。
	///		
	///		例如：/request/{**catchall}
	///		会捕获 /request/ 下的所有请求，包括多级路径后面的，以及 /request/ 路径本身。
	///		</param>
	/// <param name="agent_dst_base_uri">要代理的基 URI。例如要代理百度，则为 http://www.baidu.com/ </param>
	/// <returns></returns>
	public static WebApplication UseAgent(this WebApplication app, string routing_path, Uri agent_dst_base_uri)
	{
		app.Map(routing_path, async (HttpContext context) =>
		{
			HttpRequestMessage request_msg = await context.Request.ToHttpRequestMessage(agent_dst_base_uri);

			// 打印头部信息
			Console.WriteLine("复制到 HttpRequestMessage 前的请求头");
			context.Request.Headers.PringHeaders();
			Console.WriteLine();
			Console.WriteLine("复制到 HttpRequestMessage 后的请求头");
			request_msg.Headers.PrintHeader();
			Console.WriteLine();

			// 发送请求
			HttpClient client = app.Services.GetRequiredService<HttpClient>();
			HttpResponseMessage response_msg = await client.SendAsync(
				request_msg,
				HttpCompletionOption.ResponseHeadersRead,
				context.RequestAborted
			);

			await context.Response.SendHttpResponseMessageAsync(
				response_msg,
				context.RequestAborted
			);
		});

		return app;
	}
}
