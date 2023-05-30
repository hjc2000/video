using JSLib;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;
using TaskLib;

namespace 流式加载图片视频.Pages;
public partial class Index
{
	#region 生命周期
	public Index()
	{
		_dotnetHelper = DotNetObjectReference.Create(this);
		_initializer = new Initializer(async () =>
		{
			_jsModule = await JSModule.CreateAsync(JS, "./Pages/Index.razor.js");
			_jsOp = await JSOp.CreateAsync(JS);
			await _jsOp.AddScript("https://cdn.jsdelivr.net/npm/mpegts.js@1.7.3/dist/mpegts.min.js");
			await _jsOp.AddScript("./mux.js");
		});
	}

	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		_initializer.Start();
		await _initializer.WaitAsync();
	}
	#endregion

	private async Task Onclick()
	{
		await _initializer.WaitAsync();
		await _jsModule.InvokeVoidAsync("LoadTS", _videoElement, _dotnetHelper);
	}

	#region 工具
	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private readonly Initializer _initializer;
	DotNetObjectReference<Index> _dotnetHelper;
	#endregion

	private ElementReference _videoElement = default!;

	[JSInvokable]
	public async Task<byte[]> FetchAsync(string fileName)
	{
		using FileStream fileStream = File.Open(@"D:\my_files\workspace\wwwroot\wwwroot\" + fileName, FileMode.Open);
		byte[] buff = new byte[fileStream.Length];
		await fileStream.ReadAsync(buff);
		return buff;
	}
}
