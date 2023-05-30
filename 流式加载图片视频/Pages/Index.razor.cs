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
		await _jsModule.InvokeVoidAsync("Load_ts", _videoElement);
		//await _jsOp.LogAsync(new byte[] { 1, 2, 4, 5, 6, 7, 8, 9, });
	}

	#region 工具
	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private readonly Initializer _initializer;
	#endregion

	private ElementReference _videoElement = default!;
}
