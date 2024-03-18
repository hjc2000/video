using Microsoft.AspNetCore.Components;

namespace JCRazor.文件浏览器;
public partial class FileBrowerTitle
{
	[Parameter]
	public bool CheckBoxValue { get; set; }
	[Parameter]
	public EventCallback<bool> CheckBoxValueChanged { get; set; }

	private async Task SetCheckBoxValueAsync(bool value)
	{
		await CheckBoxValueChanged.InvokeAsync(value);
	}
}
