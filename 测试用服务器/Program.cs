using ChunkLib;
using Microsoft.AspNetCore.StaticFiles;
using System.Net;
using System.Net.Sockets;

#region 设置服务器根路径
string _webRootPath = Environment.GetEnvironmentVariable("WEBROOT") ?? "./";
_webRootPath = _webRootPath + "/wwwroot";
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
app.UseRouting();

app.MapGet("/ts.mp4", async (HttpContext context) =>
{
	try
	{
		Console.WriteLine("请求视频文件");
		context.Response.Headers.Add("Content-Disposition", "attachment; filename=\"ts.mp4\"");
		context.Response.Headers.Add("Transfer-Encoding", "chunked");
		context.Response.Headers.ContentType = "video/mp4";
		FileStream fileStream = File.OpenRead(_webRootPath + "/ts.mp4");
		await ChunkWriter.WriteContent(fileStream, context.Response.Body);
		await ChunkWriter.WriteTrailer(context.Response.Body);
	}
	catch (Exception ex)
	{
		Console.WriteLine(ex.Message);
	}
});
#endregion

#region 设置静态文件服务
/// <summary>
/// 文件content-type提供者
/// </summary>
FileExtensionContentTypeProvider provider = new();

/* 添加mime表的内容。如果不添加，provider 的默认mime表内没有dll等文件的
 * content-type，会造成浏览器接收文件后以错误的方式处理*/
provider.Mappings[".dll"] = "application/octet-stream";
provider.Mappings[".dat"] = "application/octet-stream";
provider.Mappings[".blat"] = "application/octet-stream";
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
app.MapFallbackToFile("index.html");

app.Run();