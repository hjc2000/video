using Microsoft.AspNetCore.Components;

namespace JCRazor.盒子;
/// <summary>
///		左边宽度是 max-content，右边是 1fr 的左右盒子。
/// </summary>
public partial class LeftRightBox
{
	[Parameter]
	public RenderFragment? Left { get; set; }
	[Parameter]
	public RenderFragment? Right { get; set; }

	[Parameter]
	public string ColumnsTemplate { get; set; } = "max-content 1fr";

	[Parameter]
	public string ColumnsGap { get; set; } = "5px";

	[Parameter]
	public string Height { get; set; } = "auto";
}
