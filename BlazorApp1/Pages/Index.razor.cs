using JSLib;
using Microsoft.JSInterop;

namespace BlazorApp1.Pages;

public partial class Index
{
	#region 生命周期
	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		_jsModule = await JSModule.CreateAsync(JS, "./Pages/Index.razor.js");
		_jsOp = await JSOp.CreateAsync(JS);
		await _jsOp.AddScript("https://cdn.jsdelivr.net/npm/mpegts.js@1.7.3/dist/mpegts.min.js");
		_initTask.SetResult();
	}
	#endregion

	private async Task Onclick()
	{
		await _initTask.Task;
		byte[] buffer = new byte[(int)1e6];
		using MemoryStream mstream = new(buffer);
		using DotNetStreamReference dotnetStream = new(mstream);
		await _jsModule.InvokeVoidAsync("log", dotnetStream);
	}

	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private readonly TaskCompletionSource _initTask = new();
}
