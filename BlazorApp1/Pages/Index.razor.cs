using JSLib;

namespace BlazorApp1.Pages;

public partial class Index
{
	#region 生命周期
	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		_jsModule = await JSModule.CreateAsync(JS, "./Pages/Index.razor.js");
		_jsOp = await JSOp.CreateAsync(JS);
		_callbackHelper.Action += () =>
		{
			Console.WriteLine("666666666666666666666666");
		};
		_initTask.SetResult();
	}
	#endregion
	private async Task OnClick()
	{
		await _initTask.Task;
		_inputFileElementWrapper = new(JS, _callbackHelper);
		await _inputFileElementWrapper.Click();
	}

	//private async Task OnFileLoad()
	//{
	//	await _initTask.Task;
	//	_inputFileElementWrapper = new(JS, _inputElement);
	//	await using IJSObjectReference jsFileStream = await _inputFileElementWrapper.GetFileAs_JS_Stream(0);
	//	_jsOp.Log(jsFileStream);
	//	JSStreamReader jsStreamReader = new(JS, jsFileStream);
	//	byte[] buffer = await jsStreamReader.ReadAsync();
	//	Console.WriteLine(buffer.Length);
	//	await _inputFileElementWrapper.Remove();
	//}

	private TaskCompletionSource _initTask = new();
	private InputFileElementWrapper _inputFileElementWrapper = default!;
	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;
	private CallbackHelper _callbackHelper = new();
}
