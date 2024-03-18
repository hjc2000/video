using Microsoft.AspNetCore.Components;

namespace JCRazor.表单;

/// <summary>
///		因为 input 标签的默认样式经常会导致放到一个盒子后，盒子溢出了。所以在这里封装这个组件，
///		将 input 标签设置为 box-sizing: border-box;
/// </summary>
public partial class InputElement
{
	[Parameter]
	public string Value { get; set; } = string.Empty;
	[Parameter]
	public EventCallback<string> ValueChanged { get; set; }

	private async Task OnValueChange(string value)
	{
		await ValueChanged.InvokeAsync(value);
	}
}
