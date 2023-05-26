using JsLib;
using Microsoft.AspNetCore.Components.Forms;
using Microsoft.JSInterop;

namespace BlazorApp1.Pages;
public partial class Index
{
	#region 生命周期
	public Index()
	{
		_installTask = new Task(async () =>
		{
			_module = await JsModule.CreateAsync(JS, "./Pages/Index.razor.js");
			_jsOp = await JsOp.CreateAsync(JS);
		});
	}

	protected override async Task OnInitializedAsync()
	{
		_installTask.Start();
		await _installTask;
	}
	#endregion

	private async Task Onclick()
	{
		await _installTask;
		await _jsOp.AddScript("./hls.js");
	}

	private async Task OnLoadFile(InputFileChangeEventArgs e)
	{
		await _installTask;
		Stream stream = e.File.OpenReadStream(maxAllowedSize: (long)300e6);
		DotNetStreamReference dotNetStreamReference = new(stream);
		_jsOp.Log(dotNetStreamReference);
	}

	private JsModule _module = default!;
	private JsOp _jsOp = default!;
	/// <summary>
	/// 安装本类的依赖的任务
	/// </summary>
	private readonly Task _installTask;
}
