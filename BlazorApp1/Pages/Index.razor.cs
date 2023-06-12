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

	private async Task OnClick()
	{
		await _initTask.Task;
		await using InputFileElementWrapper inputFileElementWrapper = new(JS, true);
		await inputFileElementWrapper.Click();
		int count = await inputFileElementWrapper.GetFileCount();
		Console.WriteLine($"选中了{count}个文件");
		await using IJSObjectReference jsFileStream = await inputFileElementWrapper.GetFileAs_JS_Stream(0);
		_jsOp.Log(jsFileStream);
		JSReadableStream jsReadableFileStream = new(JS, jsFileStream);
		StreamReader reader = new(jsReadableFileStream);
		Console.WriteLine(await reader.ReadToEndAsync());
	}

	private TaskCompletionSource _initTask = new();
	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
}
