HttpClient client = new()
{
	BaseAddress = new Uri("http://localhost"),
};
HttpResponseMessage msg = await client.GetAsync("ts.mp4");
if (msg.IsSuccessStatusCode)
{
	Console.WriteLine(msg.Content.Headers);
}
