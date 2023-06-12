using JSLib;
using Microsoft.AspNetCore.Components;
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
		using DotNetStreamReference dotnetStreamRef = new(stream);
		IJSObjectReference jsStream = await _jsModule.InvokeAsync<IJSObjectReference>("get_stream", dotnetStreamRef);
		_jsOp.Log(jsStream);
		JSStreamReader jsStreamReader = new(JS, jsStream);
		byte[] buffer1 = await jsStreamReader.ReadAsync();
		_jsOp.Log(buffer1);
	}

	private async Task OnFileLoad()
	{
		await _initTask.Task;
		IJSObjectReference inputFileElement = await _jsModule.InvokeAsync<IJSObjectReference>("InputFileElement.create", _inputElement);
		await inputFileElement.InvokeVoidAsync("get_file_as_stream", 0);
	}

	private ElementReference _inputElement = default!;

	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private readonly TaskCompletionSource _initTask = new();
}
