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
		});
	}

	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		_initializer.Start();
		await _initializer.WaitAsync();
		await _jsOp.AddScript("https://cdn.jsdelivr.net/npm/mpegts.js@1.7.3/dist/mpegts.min.js");
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
		using FileStream fileStream = File.Open(@"D:\my_files\workspace\音视频加工区\ts0.ts", FileMode.Open);
		DotNetStreamReference streamHelper = new(fileStream);
		await _jsModule.InvokeVoidAsync("LoadVideo", _videoElement, streamHelper);
	}

	#region 工具
	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private readonly Initializer _initializer;
	#endregion

	private ElementReference? _videoElement;
}
