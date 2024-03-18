using JCNET.流;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Primitives;

namespace HttpServerLib.扩展方法;
public static class HttpContextExtension
{
	/// <summary>
	///		根据 HttpRequest 中的信息生成一个 HttpRequestMessage，会将请求头和请求体复制到
	///		HttpRequestMessage 中。还会将客户端发过来的请求头的 Host 替换成 base_uri 中的 Host。
	/// </summary>
	/// <param name="request">客户端向本服务器发送的请求</param>
	/// <param name="base_uri">要代理请求的地址。例如要将此请求转发给百度，则传入 http://www.baidu.com/ 。</param>
	/// <returns></returns>
	public static async Task<HttpRequestMessage> ToHttpRequestMessage(this HttpRequest request, Uri base_uri)
	{
		// 准备返回值
		HttpRequestMessage request_message = new()
		{
			Method = new HttpMethod(request.Method)
		};

		// 复制请求地址信息
		UriBuilder uri_builder = new(base_uri)
		{
			Path = request.Path,
			Query = request.QueryString.ToString()
		};
		request_message.RequestUri = uri_builder.Uri;

		// 复制请求头信息
		foreach (KeyValuePair<string, StringValues> header in request.Headers)
		{
			request_message.Headers.TryAddWithoutValidation(header.Key, header.Value.ToString());
		}

		// 偷梁换柱
		request_message.Headers.Remove("Host");
		request_message.Headers.Add("Host", $"{base_uri.Host}:{base_uri.Port}");
		request_message.Headers.Remove("Referer");
		request_message.Headers.Add("Referer", uri_builder.Uri.ToString());

		// 复制请求体
		MemoryStream mstream = new();
		byte[] buf = new byte[64];
		try
		{
			// 尝试读取，如果读不到任何 1 个字节，则说明此请求没有请求体。
			int have_read = await request.Body.ReadAsync(buf);
			if (have_read > 0)
			{
				// 如果读取的字节数大于 0，说明流没有耗尽，即此请求有请求体。
				await mstream.WriteAsync(buf.AsMemory(0, have_read));
				mstream.Position = 0;

				JoinedStream jstream = new();
				jstream.AppendStream(mstream);
				jstream.AppendStream(request.Body);
				request_message.Content = new StreamContent(jstream);

				// 复制内容头部
				foreach (KeyValuePair<string, StringValues> header in request.Headers)
				{
					request_message.Content.Headers.TryAddWithoutValidation(header.Key, header.Value.ToString());
				}
			}
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
			Console.WriteLine("此请求没有请求体");
		}

		return request_message;
	}

	public static void PringHeaders(this IHeaderDictionary headers)
	{
		foreach (KeyValuePair<string, StringValues> header in headers)
		{
			Console.Write($"{header.Key}: ");
			foreach (string? value in header.Value)
			{
				Console.Write($"{value}");
			}

			Console.WriteLine();
		}
	}

	/// <summary>
	///		作为代理服务器，将帮客户端请求到的 HttpResponseMessage 返回给客户端
	/// </summary>
	/// <param name="response"></param>
	/// <param name="msg_to_sent"></param>
	/// <returns></returns>
	public static async Task SendHttpResponseMessageAsync(this HttpResponse response,
		HttpResponseMessage msg_to_sent, CancellationToken cancellationToken)
	{
		// 发送状态码
		response.StatusCode = (int)msg_to_sent.StatusCode;

		// 复制头部信息
		foreach (KeyValuePair<string, IEnumerable<string>> header in msg_to_sent.Headers)
		{
			response.Headers[header.Key] = string.Join(", ", header.Value);
		}

		// HttpResponseMessage 的头部信息分成两部分，Content 的头部信息也要复制
		foreach (KeyValuePair<string, IEnumerable<string>> header in msg_to_sent.Content.Headers)
		{
			response.Headers[header.Key] = string.Join(", ", header.Value);
		}
		// 移除此头部。直接发送响应体流，asp.net 会自动决定要不要 chunk，还是使用 http2 的分帧发送。
		response.Headers.Remove("Transfer-Encoding");
		response.Headers.Remove("Set-Cookie");

		// 发送响应体
		Stream content_stream_to_sent = await msg_to_sent.Content.ReadAsStreamAsync(cancellationToken);
		await content_stream_to_sent.CopyToAsync(response.Body, cancellationToken);
	}
}
