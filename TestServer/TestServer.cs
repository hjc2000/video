using HttpServerLib.中间件;
using Microsoft.AspNetCore.Server.Kestrel.Core;
using PInvokeLib;

/* 如何生成自签名证书，参考：
 * https://docs.azure.cn/zh-cn/articles/azure-operations-guide/application-gateway/aog-application-gateway-howto-create-self-signed-cert-via-openssl
 */

#region 设置服务器根路径
Console.WriteLine(Path.GetFullPath("./"));
string webRootPath = Environment.GetEnvironmentVariable("WEBROOT") ?? "./";
webRootPath = webRootPath.Replace("\\", "/");
Console.WriteLine(webRootPath);
#endregion

#region 创建应用
WebApplicationOptions options = new()
{
	ContentRootPath = webRootPath,
	WebRootPath = webRootPath,
};

WebApplicationBuilder builder = WebApplication.CreateBuilder(options);
builder.Services.AddHttpClient();
builder.WebHost.ConfigureKestrel((serverOptions) =>
{
	//serverOptions.ConfigureHttpsDefaults((options) =>
	//{
	//	options.ServerCertificate = new X509Certificate2("./client.pfx", "123456");
	//});

	serverOptions.ConfigureEndpointDefaults((endpointOptions) =>
	{
		endpointOptions.Protocols = HttpProtocols.Http1AndHttp2;
	});
});

WebApplication app = builder.Build();
#endregion

app.ListenOnAllIpAddresses(8848);

// 中间件
app.UseAccessControlAllowAll();
app.UseRouting();
app.UseWebSockets();
//app.UseAgent("/{**catchall}", new Uri("http://localhost:8080/"));
app.MapGet("/video", async (HttpContext context) =>
{
	// http://localhost:8848/video
	using PINativeStream video_stream = PINativeStream.GetFileStream(@"C:\Users\huang\source\repos\video\ffmpeg\资源\zf.ts");
	context.RequestAborted.Register(() =>
	{
		video_stream.Close();
		video_stream.Dispose();
	});

	try
	{
		await video_stream.CopyToAsync(context.Response.Body, context.RequestAborted);
	}
	catch (Exception ex)
	{
		Console.WriteLine(ex.ToString());
	}

	Console.WriteLine("/video 路由中间件退出。");
});

app.UseStaticFilesWithFallback();

app.Run();
