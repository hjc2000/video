HttpClient client = new()
{
	BaseAddress = new Uri("http://localhost:8848"),
};
HttpResponseMessage msg = await client.GetAsync("ts.mp4");
if (msg.IsSuccessStatusCode)
{
	foreach (KeyValuePair<string, IEnumerable<string>> head in msg.Headers)
	{
		Console.WriteLine($"{head.Key}");
		foreach (string str in head.Value)
		{
			Console.WriteLine(str);
		}

		Console.WriteLine();
	}

	FileStream file = File.Open("./ts.mp4", FileMode.OpenOrCreate);
	using (file)
	{
		Stream httpStream = await msg.Content.ReadAsStreamAsync();
		await httpStream.CopyToAsync(file);
		await file.FlushAsync();
		Console.WriteLine("完成");
	}
}

Console.ReadLine();