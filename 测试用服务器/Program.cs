using Microsoft.AspNetCore.StaticFiles;
using NetLib.HttpLib;
using System.Net;
using System.Net.Sockets;
using System.Net.WebSockets;

#region 设置服务器根路径
string _webRootPath = Environment.GetEnvironmentVariable("WEBROOT") ?? "./";
_webRootPath = _webRootPath.Replace("\\", "/");
#endregion

#region 创建应用
WebApplicationOptions options = new()
{
	ContentRootPath = _webRootPath,
	WebRootPath = _webRootPath,
};
WebApplicationBuilder builder = WebApplication.CreateBuilder(options);
WebApplication app = builder.Build();
#endregion

#region 设置服务器监听的URL
app.Urls.Clear();
app.Urls.Add("http://localhost:8848");
foreach (IPAddress ip in Dns.GetHostEntry(Dns.GetHostName()).AddressList)
{
	// 下面的判断过滤 IPv4 地址
	if (ip.AddressFamily == AddressFamily.InterNetwork)
	{
		string localIp = ip.ToString();
		Console.WriteLine(localIp);
		app.Urls.Add($"http://{localIp}:8848");
	}
}
#endregion

#region 自定义中间件
// 允许跨域
app.Use(async (HttpContext context, RequestDelegate next) =>
{
	context.Response.Headers.AccessControlAllowOrigin = "*";
	context.Response.Headers.AccessControlAllowHeaders = "*";
	context.Response.Headers.AccessControlAllowMethods = "*";
	context.Response.Headers.AccessControlExposeHeaders = "*";
	await next(context);
});
#endregion

#region 配置路由
// 放在“自定义中间件”的后面，让路由中间件在“自定义中间件”之后进行匹配，
// 也就是说一个请求会先经过“自定义中间件”的处理然后再传给路由中间件
app.UseRouting();
app.UseWebSockets();

#region 8630 代理
app.MapGet("/request", async (HttpContext context) =>
{
	Console.WriteLine(context.Request.Path);
	Console.WriteLine(context.Request.QueryString);

	HttpClient client = new()
	{
		BaseAddress = new Uri("http://192.168.0.221:9000/"),
	};

	try
	{
		HttpResponseMessage msg = await client.GetAsync("request" + context.Request.QueryString);
		if (msg.IsSuccessStatusCode)
		{
			Stream retStream = await msg.Content.ReadAsStreamAsync();
			await retStream.CopyToAsync(context.Response.Body);
		}
		else
		{
			context.Response.StatusCode = 404;
		}
	}
	catch
	{
		context.Response.StatusCode = 404;
	}
});

app.MapPost("/request", async (HttpContext context) =>
{
	Console.WriteLine(context.Request.Path);
	Console.WriteLine(context.Request.QueryString);

	HttpClient client = new()
	{
		BaseAddress = new Uri("http://192.168.0.221:9000/"),
	};

	try
	{
		HttpResponseMessage msg = await client.GetAsync("request" + context.Request.QueryString);
		if (msg.IsSuccessStatusCode)
		{
			Stream retStream = await msg.Content.ReadAsStreamAsync();
			await retStream.CopyToAsync(context.Response.Body);
		}
		else
		{
			context.Response.StatusCode = 404;
		}
	}
	catch
	{
		context.Response.StatusCode = 404;
	}
});
#endregion

app.MapGet("/ws", async (HttpContext context) =>
{
	if (context.WebSockets.IsWebSocketRequest)
	{
		using WebSocket ws = await context.WebSockets.AcceptWebSocketAsync();
		using CancellationTokenSource cts = new();
		await ws.SendAsync(new byte[] { 1, 2, 3, }, WebSocketMessageType.Binary, true, cts.Token);
		Console.WriteLine("收到websocket请求");
	}
	else
	{
		Console.WriteLine("收到的不是websocket请求");
	}
});

app.MapGet("/ts.ts", async (HttpContext context) =>
{
	try
	{
		Console.WriteLine("请求视频文件");
		//context.Response.Headers.Add("Content-Disposition", "attachment; filename=\"ts.mp4\"");
		context.Response.Headers.Add("Transfer-Encoding", "chunked");
		context.Response.Headers.ContentType = "video/mp2t";
		DateTime now = DateTime.Now;
		for (int i = 0; i <= 4; i++)
		{
			using FileStream fileStream = File.OpenRead(_webRootPath + $"/ts{i}.ts");
			await Task.Delay(6000);
			// 在这里使用 ChunkEncoder 从文件流中读取数据，写到 http 响应流中
			await ChunkEncoder.ChunkWriteContentToAsync(fileStream, context.Response.Body);
			Console.WriteLine($"发送{i}");
		}
		// 所有数据都写完了需要调用 ChunkWriteTrailerAsync 写入结束标志从而结束本次传输
		await ChunkEncoder.ChunkWriteTrailerAsync(context.Response.Body);
	}
	catch (Exception ex)
	{
		Console.WriteLine(ex.Message);
	}
});

app.MapGet("/test.txt", async (HttpContext context) =>
{
	try
	{
		using FileStream fileStream = File.Open(_webRootPath + "/test.txt", FileMode.Open);
		context.Response.Headers.Add("Transfer-Encoding", "chunked");
		context.Response.Headers.ContentType = "text/plain";
		await fileStream.ChunkWriteContentToAsync(context.Response.Body);
		await context.Response.Body.ChunkWriteTrailerAsync();
	}
	catch (Exception ex)
	{
		context.Response.StatusCode = 404;
		Console.WriteLine(ex.Message);
	}
});
#endregion

#region 设置静态文件服务
/// <summary>
/// 文件content-type提供者
/// </summary>
FileExtensionContentTypeProvider provider = new();
provider.Mappings[".m3u8"] = "application/x-mpegURL";
provider.Mappings[".ts"] = "video/MP2T ";

/// <summary>
/// 使用默认文件中间件
/// </summary>
app.UseDefaultFiles();

/// <summary>
/// 使用静态文件中间件
/// </summary>
app.UseStaticFiles(new StaticFileOptions
{
	ContentTypeProvider = provider,     //文件content-type提供者
	ServeUnknownFileTypes = true,       //对未知类型的文件提供服务
	DefaultContentType = "application/octet-stream",    //未知的文件类型的content-type
});
#endregion

/*兜底的。如果 URL 指向一个什么都没有的地址，这个地址又不是对文件的请求，就返回 index.html 给请求者。
 通过 URL 的最后一级是否含有点号可知道是否是对文件的请求。MapFallbackToFile 只对非文件请求进行重定向*/
app.MapFallbackToFile("web/index.html");
app.Run();