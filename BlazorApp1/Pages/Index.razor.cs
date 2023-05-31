using JSLib;
using Microsoft.AspNetCore.Components.Forms;
using Microsoft.JSInterop;
using TaskLib;

namespace BlazorApp1.Pages;
public partial class Index
{
	#region 生命周期
	public Index()
	{
		_initializer = new(async Task () =>
		{
			_jsModule = await JSModule.CreateAsync(JS, "./Pages/Index.razor.js");
			_jsOp = await JSOp.CreateAsync(JS);
			await _jsOp.AddScript("https://cdn.jsdelivr.net/npm/mpegts.js@1.7.3/dist/mpegts.min.js");
		});
	}

	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		_initializer.Start();
		await _initializer.WaitAsync();

	}
	#endregion

	private async Task Onclick()
	{
		await _initializer.WaitAsync();
		await _jsModule.InvokeVoidAsync("Log");
	}

	private async Task OnLoadFile(InputFileChangeEventArgs e)
	{
		await _initializer.WaitAsync();
		Stream stream = e.File.OpenReadStream(maxAllowedSize: (long)300e6);
		DotNetStreamReference dotNetStreamReference = new(stream);
		_jsOp.Log(dotNetStreamReference);
	}

	private JSModule _jsModule = default!;
	private JSOp _jsOp = default!;

	/// <summary>
	/// 安装本类的依赖的任务
	/// </summary>
	private readonly Initializer _initializer;
}
