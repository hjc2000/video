using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Http.Features;
using Microsoft.AspNetCore.StaticFiles;
using Microsoft.Net.Http.Headers;
using System.Net;
using System.Net.Sockets;

namespace HttpServerLib.中间件;
public static class 通用中间件
{
	/// <summary>
	///		在指定端口监听所有 IP 地址
	/// </summary>
	/// <param name="app"></param>
	/// <param name="port"></param>
	/// <returns></returns>
	public static WebApplication ListenOnAllIpAddresses(this WebApplication app, ushort port)
	{
		app.Urls.Clear();
		bool has_loopback_ip = false;
		foreach (IPAddress ip in Dns.GetHostEntry(Dns.GetHostName()).AddressList)
		{
			if (ip.AddressFamily is not (AddressFamily.InterNetwork or AddressFamily.InterNetworkV6))
			{
				// 如果不是 IPV4 地址或 IPV6 地址，跳过
				continue;
			}

			// 如果是 IPV4 地址或 IPV6 地址
			string local_ip = ip.ToString();
			if (IPAddress.IsLoopback(ip))
			{
				has_loopback_ip = true;
			}

			Console.WriteLine(local_ip);
			app.Urls.Add($"http://{local_ip}:{port}");
			//app.Urls.Add($"https://{local_ip}:{port + 1}");
		}

		/* 最后检查有没有添加 localhost，没有的话需要补充。
		 * 在 linux 中，执行完上面的循环后，localhost 也被添加了，在 windows 中，执行完上面的
		 * 循环，localhost 没有被添加。如果下面不加判断，直接添加 localhost，在 linux 中就会
		 * 出现重复监听导致的异常。
		 */
		if (!has_loopback_ip)
		{
			app.Urls.Add($"http://localhost:{port}");
			//app.Urls.Add($"https://localhost:{port + 1}");
		}

		return app;
	}

	/// <summary>
	///		解除请求体大小的限制
	/// </summary>
	/// <param name="app"></param>
	/// <returns></returns>
	public static WebApplication UseUnlimitedRequestBodySize(this WebApplication app)
	{
		app.Use(async (context, next) =>
		{
			IHttpMaxRequestBodySizeFeature? features = context.Features.Get<IHttpMaxRequestBodySizeFeature>();
			if (features is not null)
			{
				features.MaxRequestBodySize = null;
			}

			await next(context);
		});

		return app;
	}

	/// <summary>
	///		使用静态文件服务，并且还有兜底，及找不到的 URL 都会返回 fall_back_file
	/// </summary>
	/// <param name="app"></param>
	/// <param name="fall_back_file"></param>
	/// <returns></returns>
	public static WebApplication UseStaticFilesWithFallback(
		this WebApplication app,
		string fall_back_file = "index.html"
	)
	{
		FileExtensionContentTypeProvider provider = new();
		provider.Mappings[".m3u8"] = "application/x-mpegURL";
		provider.Mappings[".ts"] = "video/MP2T";
		app.UseDefaultFiles();
		app.UseStaticFiles(new StaticFileOptions
		{
			ContentTypeProvider = provider,     //文件content-type提供者
			ServeUnknownFileTypes = true,       //对未知类型的文件提供服务
			DefaultContentType = "application/octet-stream",    //未知的文件类型的content-type
			OnPrepareResponse = (ctx) =>
			{
				ctx.Context.Response.Headers[HeaderNames.AcceptRanges] = "bytes";
			},
		});

		/*兜底的。如果 URL 指向一个什么都没有的地址，这个地址又不是对文件的请求，就返回 index.html 给请求者。
		 通过 URL 的最后一级是否含有点号可知道是否是对文件的请求。MapFallbackToFile 只对非文件请求进行重定向*/
		app.MapFallbackToFile(fall_back_file);
		return app;
	}
}
