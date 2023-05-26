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
		_jsOp = await JsOp.CreateAsync(JS);
	}
	#endregion

	private async Task Onclick()
	{
		if (_module == null)
		{
			return;
		}

		if (_jsOp == null)
		{
			return;
		}

		await _jsOp.AddScript("./hls.js");
		await _module.InvokeVoidAsync("Log");
	}

	private JsHelper<Index>? _module;
	private JsOp? _jsOp;
	private ElementReference? _img;
}
