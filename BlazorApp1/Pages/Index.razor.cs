using JsLib;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace BlazorApp1.Pages;
public partial class Index
{
	#region 生命周期
	protected override async Task OnInitializedAsync()
	{
		_module = await JsHelper<Index>.CreateAsync(JS, "./Pages/Index.razor.js", this);
		await _module.InvokeVoidAsync("AddScript");
	}
	#endregion

	private async Task Onclick()
	{
		if (_module == null)
		{
			return;
		}

		await _module.InvokeVoidAsync("Log");
	}

	#region 工具
	private JsHelper<Index>? _module;
	#endregion
	private ElementReference? _img;
}
