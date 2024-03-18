using System.Net;

namespace HttpServerLib.HTTP请求响应头部信息;

/// <summary>
///		http 响应的头部。即 HttpResponseMessage 类所储存的头部信息。
/// </summary>
public class HttpResponseHead
{
	public HttpResponseHead() { }

	public HttpResponseHead(HttpResponseMessage msg)
	{
		StatusCode = msg.StatusCode;
		Version = msg.Version;
		Headers = new HttpHeaderDictionary(msg.Headers);
		ContentHeaders = new HttpHeaderDictionary(msg.Content.Headers);
	}

	public HttpResponseMessage CreateHttpResponseMessage(Stream response_content_stream)
	{
		HttpResponseMessage response_message = new()
		{
			StatusCode = StatusCode,
			Version = Version
		};
		response_message.Headers.Clear();
		foreach (KeyValuePair<string, List<string>> header in Headers)
		{
			response_message.Headers.TryAddWithoutValidation(header.Key, header.Value);
		}

		response_message.Content = new StreamContent(response_content_stream);
		response_message.Content.Headers.Clear();
		foreach (KeyValuePair<string, List<string>> header in ContentHeaders)
		{
			response_message.Content.Headers.TryAddWithoutValidation(header.Key, header.Value);
		}

		return response_message;
	}

	public HttpStatusCode StatusCode { get; set; }
	public bool IsSuccessStatusCode
	{
		get
		{
			return (int)StatusCode is >= 200 and <= 299;
		}
	}
	public Version Version { get; set; } = new Version();
	public HttpHeaderDictionary Headers { get; set; } = [];
	public HttpHeaderDictionary ContentHeaders { get; set; } = [];
}
