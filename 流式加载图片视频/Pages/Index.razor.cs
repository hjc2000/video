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
		if (_videoElement == null)
		{
			return;
		}

		await _initializer.WaitAsync();
		await _jsModule.InvokeVoidAsync("Log");
		await _jsModule.InvokeVoidAsync("LoadVideoFromUrl", _videoElement, "http://192.168.0.221:8000/101.ts");
	}

	#region 工具
	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private readonly Initializer _initializer;
	#endregion

	private ElementReference? _videoElement;
}
