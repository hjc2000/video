HttpClient client = new()
{
	BaseAddress = new Uri("http://localhost:8848"),
};
HttpResponseMessage msg = await client.GetAsync("ts.mp4");
if (msg.IsSuccessStatusCode)
{
	Console.WriteLine(msg.Headers);
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