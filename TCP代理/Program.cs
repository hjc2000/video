using System.Net;
using System.Net.Sockets;

IPEndPoint localEp = new(IPAddress.Loopback, 0);
TcpListener tcpListener = new(localEp);
tcpListener.Start();
Console.WriteLine("启动");
Console.WriteLine($"使用浏览器打开：http://{tcpListener.LocalEndpoint}/");
while (true)
{
	TcpClient client = await tcpListener.AcceptTcpClientAsync();
	Console.WriteLine($"收到TCP连接请求，请求方 IP 地址：{client.Client.RemoteEndPoint?.ToString()}");
	HandleClient(client);
}

static async void HandleClient(TcpClient client)
{
	TcpClient baiduClient = new();
	await baiduClient.ConnectAsync("localhost", 8848);
	NetworkStream localClientStream = client.GetStream();
	NetworkStream remoteClientStream = baiduClient.GetStream();
	List<Task> tasks = new()
	{
		CopyStream(localClientStream, remoteClientStream),
		CopyStream(remoteClientStream, localClientStream)
	};
	await Task.WhenAll(tasks);
	client.Close();
	baiduClient.Close();
	Console.WriteLine("连接关闭");
}

static async Task CopyStream(Stream src_stream, Stream dest_stream)
{
	byte[] read_buf = new byte[1500];
	while (true)
	{
		Console.WriteLine("复制");
		int have_read = await src_stream.ReadAsync(read_buf);
		if (have_read == 0) break;
		await dest_stream.WriteAsync(read_buf, 0, have_read);
	}
}
