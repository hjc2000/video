using HttpServerLib.Websocket连接池;
using HttpServerLib.WsHttpAgent;
using HttpServerLib.中间件;

#region 设置服务器根路径
Console.WriteLine(Path.GetFullPath("./"));
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
builder.Services.AddSingleton<WebsocketConnectionPool>();
builder.Services.AddSingleton<IWebsocketUserIdParser, WebsocketUserIdParser>();
builder.Services.AddHttpClient();
WebApplication app = builder.Build();
#endregion

app.ListenOnAllIpAddresses(8848);

// 使用中间件
app.UseAccessControlAllowAll();
app.UseRouting();
app.UseWebSockets();
app.UseWebsocketConnectionPool();// 使用 websocket 连接池中间件
app.UseWsHttpAgent("/request/{**catchall}", new Uri("http://192.168.0.221:9000/"));
app.UseStaticFilesWithFallback("/web/index.html");

app.Run();

internal class WebsocketUserIdParser : IWebsocketUserIdParser
{
	public string ParseUserId(HttpContext context)
	{
		return "user_id";
	}
}