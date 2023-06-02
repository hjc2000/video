using JSLib;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;
using StreamLib;
using System.Net.WebSockets;

namespace 流式加载图片视频.Pages;
public partial class Index
{
	#region 生命周期
	public Index()
	{
		_dotnetHelper = DotNetObjectReference.Create(this);
		_splicedStream.OnStreamQueueEmpty += async (tcs) =>
		{
			if (_tsIndex <= 4)
			{
				_jsOp.Log($"读取 ts{_tsIndex}.ts");
				Stream fileStream = await FileSystem.OpenAppPackageFileAsync($"ts{_tsIndex++}.ts");
				_splicedStream.PushBack(fileStream);
			}

			tcs.SetResult();
		};
	}

	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		_jsModule = await JSModule.CreateAsync(JS, "./Pages/Index.razor.js");
		_jsOp = await JSOp.CreateAsync(JS);
		await _jsOp.AddScript("https://cdn.jsdelivr.net/npm/mpegts.js@1.7.3/dist/mpegts.min.js");
		await _jsOp.AddScript("./mux.js");
		_initTask.SetResult();
	}
	#endregion

	private async Task Onclick()
	{
		await _initTask.Task;
		await _jsModule.InvokeVoidAsync("LoadTS", _videoElement, _dotnetHelper);
	}

	private async Task OnRequestWebsocket()
	{
		using ClientWebSocket ws = new();
		using CancellationTokenSource cts = new();
		await ws.ConnectAsync(new Uri(@"ws://localhost:8848/ws"), cts.Token);
		byte[] buff = new byte[1024];
		WebSocketReceiveResult result = await ws.ReceiveAsync(buff, cts.Token);
		for (int i = 0; i < result.Count; i++)
		{
			await _jsOp.LogAsync(buff[i]);
		}
	}

	#region 工具
	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private readonly DotNetObjectReference<Index> _dotnetHelper;
	private readonly TaskCompletionSource _initTask = new();
	private readonly SplicedStream _splicedStream = new();
	#endregion

	#region 私有字段
	private int _tsIndex = 0;
	private readonly byte[] _tsBuff = new byte[(int)20e6];
	#endregion

	private ElementReference _videoElement = default!;

	[JSInvokable]
	public async Task<byte[]> FetchAsync()
	{
		int haveRead = await _splicedStream.ReadAsync(_tsBuff, 0, _tsBuff.Length);
		_jsOp.Log($"读取了{haveRead}字节");
		if (haveRead > 0)
		{
			byte[] buff = new byte[haveRead];
			Array.Copy(_tsBuff, buff, haveRead);
			return buff;
		}
		else
		{
			throw new Exception("流到达尽头");
		}
	}
}
