using System.Net.Http.Headers;

namespace HttpClientLib.扩展方法;
public static class HttpMsgExtension
{
	/// <summary>
	/// 在控制台中打印出标头信息
	/// </summary>
	/// <param name="headers"></param>
	public static void PrintHeader(this HttpHeaders headers)
	{
		foreach (KeyValuePair<string, IEnumerable<string>> header in headers)
		{
			Console.Write($"{header.Key}: ");
			string header_value = string.Join(",", header.Value);
			Console.WriteLine(header_value);
		}
	}

	/// <summary>
	/// 将 HttpRequestMessage 对象里面的关键属性复制到新的 HttpRequestMessage 对象中，
	/// 然后可以将这个新的 HttpRequestMessage 对象发送出去。为什么要这么做？因为
	/// HttpRequestMessage 对象只能被发送一次，发送完了之后再次发送会引发异常。
	/// </summary>
	/// <param name="msg"></param>
	/// <returns></returns>
	public static HttpRequestMessage CloneForResending(this HttpRequestMessage msg)
	{
		HttpRequestMessage request_msg = new()
		{
			RequestUri = msg.RequestUri,
			Method = msg.Method,
			Content = msg.Content,
			Version = msg.Version,
			VersionPolicy = msg.VersionPolicy,
		};

		foreach (KeyValuePair<string, IEnumerable<string>> header in msg.Headers)
		{
			request_msg.Headers.Add(header.Key, string.Join(',', header.Value));
		}

		return request_msg;
	}
}
