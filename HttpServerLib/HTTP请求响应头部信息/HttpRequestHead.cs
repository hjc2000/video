using System.Text.Json;

namespace HttpServerLib.HTTP请求响应头部信息;

/// <summary>
///		对应 HttpRequestMessage 类所储存的头部信息。即 http 请求的头部信息。包括请求行和标头，
///		但是不包括请求体。请求体需要另外作为流来传输，这里只有请求体的标头信息。
/// </summary>
public class HttpRequestHead
{
	/// <summary>
	///		为了能够从 json 反序列化为本类对象，必须有一个公共的无参构造函数
	/// </summary>
	public HttpRequestHead() { }

	public HttpRequestHead(HttpRequestMessage request)
	{
		RequestUri = request.RequestUri;
		Method = request.Method;
		Headers = new HttpHeaderDictionary(request.Headers);
		Version = request.Version;
		VersionPolicy = request.VersionPolicy;
		if (request.Content is not null)
		{
			ContentHeaders = new HttpHeaderDictionary(request.Content.Headers);
		}
	}

	/// <summary>
	///		创造一个 HttpRequestMessage 对象。
	/// </summary>
	/// <param name="request_content">
	///		可能包含有请求体的流（也可能没有请求体）。
	///		
	///		<br/> 本对象包含的是请求的头部信息，要形成完整的 HttpRequestMessage 对象，还需要请求体。
	///		然而，不是所有请求都有请求体。但好在没有请求体时，本对象的 ContentHeaders 属性为空。所以
	///		尽管将可能含有请求体的流传进来。
	///		
	///		<br/> 当本对象的 ContentHeaders 属性为空时，本函数不会 new 一个 StreamContent。
	///		如果本对象的 ContentHeaders 属性不为空，说明此次请求有请求体，则会 new 一个
	///		StreamContent 对象，将 request_content 包起来。
	/// </param>
	/// <returns></returns>
	public HttpRequestMessage CreateHttpRequestMessage(Stream? request_content)
	{
		HttpRequestMessage http_request_message = new()
		{
			RequestUri = RequestUri,
			Method = Method,
			Version = Version,
			VersionPolicy = VersionPolicy
		};

		// 复制头部信息
		http_request_message.Headers.Clear();
		foreach (KeyValuePair<string, List<string>> header in Headers)
		{
			http_request_message.Headers.TryAddWithoutValidation(header.Key, header.Value);
		}

		// 复制请求体头部信息和请求体
		if (ContentHeaders is not null)
		{
			if (request_content is null)
			{
				throw new ArgumentNullException(
					nameof(request_content),
					$"本对象的 {nameof(ContentHeaders)} 属性不为空，说明有请求体，但是 {nameof(request_content)} 却为空"
				);
			}

			// 有请求体且 request_content 不为空
			http_request_message.Content = new StreamContent(request_content);
			http_request_message.Content.Headers.Clear();
			foreach (KeyValuePair<string, List<string>> header in ContentHeaders)
			{
				http_request_message.Content.Headers.TryAddWithoutValidation(header.Key, header.Value);
			}
		}

		return http_request_message;
	}

	public Uri? RequestUri { get; set; }
	public HttpMethod Method { get; set; } = HttpMethod.Get;
	public Version Version { get; set; } = new Version(1, 1);
	public HttpVersionPolicy VersionPolicy { get; set; }
	public HttpHeaderDictionary Headers { get; set; } = [];

	/// <summary>
	///		请求体的头部信息。如果没有请求体，此属性为空。
	/// </summary>
	public HttpHeaderDictionary? ContentHeaders { get; set; }

	/// <summary>
	///		将本对象序列化为 json 字符串。
	/// </summary>
	/// <returns></returns>
	public string ToJsonString()
	{
		string json_string = JsonSerializer.Serialize(this, new JsonSerializerOptions
		{
			AllowTrailingCommas = true,
			WriteIndented = true,
		});

		return json_string;
	}

	/// <summary>
	///		将本对象序列化为 json 同时写入 dst_stream 中。
	/// </summary>
	/// <param name="dst_stream"></param>
	/// <param name="cancellationToken"></param>
	/// <returns></returns>
	public async Task SerializeToJsonStreamAsync(Stream dst_stream, CancellationToken cancellationToken)
	{
		await JsonSerializer.SerializeAsync(
			dst_stream,
			this,
			(JsonSerializerOptions?)null,
			cancellationToken
		);
	}
}
