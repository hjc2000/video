using Microsoft.AspNetCore.Components;

namespace JCRazor.盒子;
/// <summary>
///		默认是提供一个从左到右的堆栈视图，每个列的宽度为 max-content，列之间有间隙。
///		溢出后会自动滚动。
/// </summary>
public partial class HorizontalStackView
{
	[Parameter]
	public RenderFragment? ChildContent { get; set; }

	/// <summary>
	///		由于 css 的限制，列数无法像行数那样自动适应元素数量。本组件默认的 ColumnCount
	///		是 2，如果超过 2 个元素，会自动跑到下一行去，而不会产生第 3 列。唯一的方法就是
	///		显式声明你需要几列。
	/// </summary>
	[Parameter]
	public int ColumnCount { get; set; } = 2;

	/// <summary>
	///		网格间距默认 1em。
	/// </summary>
	[Parameter]
	public string Gap { get; set; } = "1em";

	[Parameter]
	public string Padding { get; set; } = "0";

	[Parameter]
	public string Width { get; set; } = "100%";
	[Parameter]
	public string Height { get; set; } = "100%";

	/// <summary>
	///		堆栈视图中的每一列默认左对齐
	/// </summary>
	[Parameter]
	public string JustifyContent { get; set; } = "start";
	/// <summary>
	///		每一列的网格线中的元素默认居中对齐
	/// </summary>
	[Parameter]
	public string JustifyItems { get; set; } = "center";

	[Parameter]
	public string Overflow { get; set; } = "auto";
}
