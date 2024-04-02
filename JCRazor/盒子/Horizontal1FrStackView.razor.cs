using Microsoft.AspNetCore.Components;

namespace JCRazor.盒子;

/// <summary>
///		横向的均分每一列宽度的堆栈。每一列的网格中的元素居中对齐。
/// </summary>
public partial class Horizontal1FrStackView
{
	[Parameter]
	public RenderFragment? ChildContent { get; set; }

	[Parameter]
	public int ColumnCount { get; set; } = 2;

	[Parameter]
	public string Gap { get; set; } = "1em";
}
