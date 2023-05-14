using StringLib;

HttpClient client = new();
client.DefaultRequestHeaders.Add("Host", "live.qztv.cn");
client.DefaultRequestHeaders.Add("Origin", "https://www.qztv.cn");
client.DefaultRequestHeaders.Add("Referer", "https://www.qztv.cn/");
string url = "https://live.qztv.cn/live/mny.m3u8?auth_key=1681739082-0-0-bd1bf1fb7ac25b1ef497c0804096327a";
HttpResponseMessage message = await client.GetAsync(url);
if (message.IsSuccessStatusCode)
{
	string m3u8 = await message.Content.ReadAsStringAsync();
	int i = 0;
	while (true)
	{
		(string line, m3u8) = m3u8.ReadLine();
		if (line != string.Empty)
		{
			if (line.EndsWith("ts"))
			{
				string uri = "https://live.qztv.cn/live/" + line;
				message = await client.GetAsync(uri);
				if (message.IsSuccessStatusCode)
				{
					Console.WriteLine($"正在保存ts{i}");
					FileStream file = File.Open($"./ts{i++}.ts",
						FileMode.Create,
						FileAccess.ReadWrite,
						FileShare.ReadWrite);
					using (file)
					{
						Stream stream = await message.Content.ReadAsStreamAsync();
						await stream.CopyToAsync(file);
					}
				}
			}
		}
		else
		{
			Console.WriteLine("下载完成");
			break;
		}
	}
}
else
{
	Console.WriteLine("连接失败");
}

Console.ReadLine();