using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace 流式加载图片视频.Pages;
public partial class Index
{
	#region 生命周期
	protected override async Task OnInitializedAsync()
	{
		module = await JS.InvokeAsync<IJSObjectReference>("import",
			"./Pages/Index.razor.js");
	}
	#endregion

	private async Task Onclick()
	{
		Stream stream = await FileSystem.Current.OpenAppPackageFileAsync("test.jpg");
		DotNetStreamReference dotnetStreamRef = new(stream);
		await module.InvokeVoidAsync("setImage", _img, dotnetStreamRef);
	}

	private IJSObjectReference module = default!;
	private ElementReference _img = default!;
}
