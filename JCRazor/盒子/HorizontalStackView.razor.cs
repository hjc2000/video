using Microsoft.AspNetCore.Components;

namespace JCRazor.盒子;
/// <summary>
///		* 默认是提供一个从左到右的堆栈视图，每个列的宽度为 max-content，列之间有间隙。
///		* 溢出后会自动滚动。
/// </summary>
public partial class HorizontalStackView
{
	[Parameter]
	public RenderFragment? ChildContent { get; set; }

	[Parameter]
	public int ColumnCount { get; set; } = 2;

	[Parameter]
	public string Gap { get; set; } = "1em";

	[Parameter]
	public string Padding { get; set; } = "0";

	[Parameter]
	public string Width { get; set; } = "100%";
	[Parameter]
	public string Height { get; set; } = "100%";

	[Parameter]
	public string JustifyContent { get; set; } = "start";
	[Parameter]
	public string JustifyItems { get; set; } = "center";

	[Parameter]
	public string Overflow { get; set; } = "auto";
}
