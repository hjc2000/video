HttpClient client = new()
{
	BaseAddress = new Uri("https://live.qztv.cn/live/"),
};
client.DefaultRequestHeaders.Add("Host", "live.qztv.cn");
client.DefaultRequestHeaders.Add("Origin", "https://www.qztv.cn");
client.DefaultRequestHeaders.Add("Referer", "https://www.qztv.cn/");
string url = "mny.m3u8?auth_key=1681739082-0-0-bd1bf1fb7ac25b1ef497c0804096327a";
HttpResponseMessage msg = await client.GetAsync(url);
if (!msg.IsSuccessStatusCode)
{
	Console.WriteLine("请求失败");
}
else
{
	using Stream m3u8 = await msg.Content.ReadAsStreamAsync();
	StreamReader reader = new(m3u8);
	int i = 0;
	while (true)
	{
		string? line = await reader.ReadLineAsync();
		if (line == null)
		{
			Console.WriteLine("下载完成");
			break;
		}

		if (line.EndsWith("ts"))
		{
			msg = await client.GetAsync(line);
			if (msg.IsSuccessStatusCode)
			{
				Console.WriteLine($"正在保存ts{i}");
				using FileStream file = File.Open($"./ts{i++}.ts",
					FileMode.Create,
					FileAccess.ReadWrite,
					FileShare.ReadWrite);
				using Stream stream = await msg.Content.ReadAsStreamAsync();
				await stream.CopyToAsync(file);
			}
		}
	}
}

Console.ReadLine();