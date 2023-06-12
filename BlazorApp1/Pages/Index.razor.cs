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

	private async Task OnFileLoad()
	{
		await _initTask.Task;
		IJSObjectReference inputFileElement = await _jsModule.InvokeAsync<IJSObjectReference>("InputFileElement.create", _inputElement);
		IJSObjectReference jsFileStream = await inputFileElement.InvokeAsync<IJSObjectReference>("get_file_as_stream", 0);
		JSStreamReader jsStreamReader = new(JS, jsFileStream);
		byte[] buffer = await jsStreamReader.ReadAsync();
		Console.WriteLine(buffer.Length);
	}

	private ElementReference _inputElement = default!;

	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private readonly TaskCompletionSource _initTask = new();
}
