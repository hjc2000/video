using Microsoft.AspNetCore.Builder;

namespace HttpServerLib.中间件;
public static class AccessControl
{
	/// <summary>
	///		将所有 AccessControl 的选项全部设置为 *
	/// </summary>
	/// <param name="app"></param>
	/// <returns></returns>
	public static WebApplication UseAccessControlAllowAll(this WebApplication app)
	{
		app.Use(async (context, next) =>
		{
			context.Response.Headers.AccessControlAllowOrigin = "*";
			context.Response.Headers.AccessControlAllowHeaders = "*";
			context.Response.Headers.AccessControlAllowMethods = "*";
			context.Response.Headers.AccessControlExposeHeaders = "*";
			await next(context);
		});

		return app;
	}

	public static WebApplication UseAccessControl(
		this WebApplication app,
		string origin,
		string methods,
		string headers,
		string expose_headers
	)
	{
		app.Use(async (context, next) =>
		{
			context.Response.Headers.AccessControlAllowOrigin = origin;
			context.Response.Headers.AccessControlAllowMethods = methods;
			context.Response.Headers.AccessControlAllowHeaders = headers;
			context.Response.Headers.AccessControlExposeHeaders = expose_headers;
			await next(context);
		});

		return app;
	}
}
