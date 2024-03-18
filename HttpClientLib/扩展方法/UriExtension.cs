namespace HttpClientLib.扩展方法;
public static class UriExtension
{
	/// <summary>
	/// 从类似于 http://localhost:80/web 的 uri 提取出 http://localhost:80/
	/// </summary>
	/// <param name="uri"></param>
	/// <returns></returns>
	public static Uri GetRootUri(this Uri uri)
	{
		UriBuilder builder = new(uri);
		return new Uri($"{builder.Scheme}://{builder.Host}:{builder.Port}/");
	}

	/// <summary>
	/// 从类似于 http://localhost:80/web 的 uri 提取出 http://localhost:80/
	/// </summary>
	/// <param name="uri"></param>
	/// <returns></returns>
	public static Uri GetRootUri(this string uri)
	{
		UriBuilder builder = new(uri);
		return new Uri($"{builder.Scheme}://{builder.Host}:{builder.Port}/");
	}
}
