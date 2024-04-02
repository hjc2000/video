using Microsoft.AspNetCore.Components;

namespace JCRazor.盒子;
/// <summary>
///		左边宽度是 max-content，右边是 1fr 的左右盒子。
/// </summary>
public partial class LeftRightBox
{
	/// <summary>
	///		左侧内容
	/// </summary>
	[Parameter]
	public RenderFragment? Left { get; set; }
	/// <summary>
	///		右侧内容
	/// </summary>
	[Parameter]
	public RenderFragment? Right { get; set; }

	/// <summary>
	///		列模板，默认 max-content 1fr
	/// </summary>
	[Parameter]
	public string ColumnsTemplate { get; set; } = "max-content 1fr";

	/// <summary>
	///		列间距默认 5px
	/// </summary>
	[Parameter]
	public string ColumnsGap { get; set; } = "5px";

	/// <summary>
	///		宽度默认 100%
	/// </summary>
	[Parameter]
	public string Width { get; set; } = "100%";
	/// <summary>
	///		高度默认 auto
	/// </summary>
	[Parameter]
	public string Height { get; set; } = "auto";
}
