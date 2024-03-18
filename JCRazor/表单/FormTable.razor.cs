using Microsoft.AspNetCore.Components;

namespace JCRazor.表单;
public partial class FormTable
{
	[Parameter]
	public RenderFragment? Head { get; set; }

	[Parameter]
	public RenderFragment? Body { get; set; }

	[Parameter]
	public RenderFragment? Foot { get; set; }

	[Parameter]
	public string HeadPadding { get; set; } = "0";
	[Parameter]
	public string BodyPadding { get; set; } = "0";
	[Parameter]
	public string FootPadding { get; set; } = "0";

	[Parameter]
	public string BodyColumnGap { get; set; } = "1em";
	[Parameter]
	public string BodyRowGap { get; set; } = "15px";

	/// <summary>
	///		主体部分网格的列模板，默认值是："max-content 1fr"。
	/// </summary>
	[Parameter]
	public string BodyGridTemplateColumns { get; set; } = "max-content 1fr";

	[Parameter]
	public string BodyJustifyContent { get; set; } = "stretch";
	[Parameter]
	public string BodyJustifyItems { get; set; } = "stretch";

	[Parameter]
	public string BodyAlignContent { get; set; } = "start";
	[Parameter]
	public string BodyAlignItems { get; set; } = "stretch";
}
