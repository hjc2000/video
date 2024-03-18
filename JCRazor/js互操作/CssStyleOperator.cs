using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace JCRazor.js互操作;

public class CssStyleOperator
{
	public CssStyleOperator(IJSRuntime jsrt)
	{
		_jsm = new JSModule(jsrt, "./_content/JCRazor/CssStyleOperator.js");
	}

	private JSModule _jsm;

	public async Task<string> GetComputedStyleAsync(ElementReference element, string style_name)
	{
		return await _jsm.InvokeAsync<string>("CssStyleOperator.get_computed_style", element, style_name);
	}

	public async Task SetWidthAsync(ElementReference element, string width)
	{
		await _jsm.InvokeVoidAsync("CssStyleOperator.set_width", element, width);
	}

	public async Task SetHeightAsync(ElementReference element, string height)
	{
		await _jsm.InvokeVoidAsync("CssStyleOperator.set_height", element, height);
	}

	public async Task SetPaddingAsync(ElementReference element, string padding)
	{
		await _jsm.InvokeVoidAsync("CssStyleOperator.set_padding", element, padding);
	}

	public async Task SetMarginAsync(ElementReference element, string margin)
	{
		await _jsm.InvokeVoidAsync("CssStyleOperator.set_margin", element, margin);
	}
}
