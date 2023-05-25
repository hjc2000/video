using JsLib;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace 流式加载图片视频.Pages;
public partial class Index
{
	#region 生命周期
	protected override async Task OnInitializedAsync()
	{
		_hlsJs = await JsHelper<Index>.CreateAsync(JS, "https://cdn.jsdelivr.net/npm/hls.js@1", this);
		_module = await JS.InvokeAsync<IJSObjectReference>("import",
			"./Pages/Index.razor.js");
	}
	#endregion

	private async Task Onclick()
	{
		if (_module == null)
		{
			return;
		}

		using Stream stream = await FileSystem.Current.OpenAppPackageFileAsync("test.jpg");
		DotNetStreamReference dotnetStreamRef = new(stream);
		await _module.InvokeVoidAsync("setImage", _img, dotnetStreamRef);
		if (_hlsJs == null)
		{
			return;
		}

		bool support = await _hlsJs.InvokeAsync<bool>("Hls.isSupported");
	}

	#region 工具
	private JsHelper<Index>? _hlsJs;
	private IJSObjectReference? _module;
	#endregion
	private ElementReference? _img;
}
