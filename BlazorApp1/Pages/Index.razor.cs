using JSLib;

namespace BlazorApp1.Pages;

public partial class Index
{
	#region 生命周期
	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		_jsm = await JSModule.CreateAsync(JS, "./Pages/Index.razor.js");
		_jsop = await JSOp.CreateAsync(JS);
		_initTask.SetResult();
	}
	#endregion

	private async Task OnClick()
	{
		await _jsop.DownloadFromUrl("http://localhost:8848/qq.mp4");
	}

	private TaskCompletionSource _initTask = new();
	private JSModule _jsm = default!;
	private JSOp _jsop = default!;
}
