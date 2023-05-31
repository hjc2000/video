using JSLib;
using Microsoft.AspNetCore.Components.Forms;
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
		await _jsModule.InvokeVoidAsync("Log");
	}

	private async Task OnLoadFile(InputFileChangeEventArgs e)
	{
		await _initTask.Task;
		Stream stream = e.File.OpenReadStream(maxAllowedSize: (long)300e6);
		DotNetStreamReference dotNetStreamReference = new(stream);
		_jsOp.Log(dotNetStreamReference);
	}

	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private readonly TaskCompletionSource _initTask = new();
}
