using Microsoft.AspNetCore.Components;

namespace JCRazor.盒子;
public partial class VerticalStackView
{
	[Parameter]
	public RenderFragment? ChildContent { get; set; }

	[Parameter]
	public string RowGap { get; set; } = "0";

	[Parameter]
	public string Overflow { get; set; } = "auto";

	[Parameter]
	public string Width { get; set; } = "100%";
	[Parameter]
	public string Height { get; set; } = "100%";

	[Parameter]
	public string Padding { get; set; } = "0";

	[Parameter]
	public string AlignContent { get; set; } = "start";
	[Parameter]
	public string AlignItems { get; set; } = "center";
}
