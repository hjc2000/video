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
		_initTask.SetResult();
	}
	#endregion

	private async Task Onclick()
	{
		await _initTask.Task;
		byte[] buffer = new byte[1024];
		using MemoryStream stream = new(buffer);
		using DotNetStreamReference dotnetStream = new(stream);
		IJSObjectReference jsStream = await _jsModule.InvokeAsync<IJSObjectReference>("get_stream", dotnetStream);
		IJSObjectReference reader = await jsStream.InvokeAsync<IJSObjectReference>("getReader");
		_jsOp.Log(reader);
		JSStreamReader jsStreamReader = new(JS, reader);
		ReadResult readResult = await jsStreamReader.ReadAsync();
		Console.WriteLine(readResult);
	}

	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private readonly TaskCompletionSource _initTask = new();
}
