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
		InputFileElementWrapper wrapper = new(JS, _inputElement);
		IJSObjectReference jsFileStream = await wrapper.GetFileAs_JS_Stream(0);
		_jsOp.Log(jsFileStream);
		JSStreamReader jsStreamReader = new(JS, jsFileStream);
		byte[] buffer = await jsStreamReader.ReadAsync();
		Console.WriteLine(buffer.Length);
		await wrapper.Remove();
	}

	private ElementReference _inputElement = default!;

	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private readonly TaskCompletionSource _initTask = new();
}
