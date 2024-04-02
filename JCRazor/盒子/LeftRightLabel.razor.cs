using Microsoft.AspNetCore.Components;

namespace JCRazor.盒子;

/// <summary>
///		左边宽度是 max-content，右边宽度是 1fr 的 label。
/// </summary>
public partial class LeftRightLabel
{
	[Parameter]
	public RenderFragment? Left { get; set; }
	[Parameter]
	public RenderFragment? Right { get; set; }

	/// <summary>
	///		默认是 max-content 1fr
	/// </summary>
	[Parameter]
	public string ColumnTemplate { get; set; } = "max-content 1fr";
	/// <summary>
	///		列间距默认 5px
	/// </summary>
	[Parameter]
	public string ColumnGap { get; set; } = "5px";

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
