using Microsoft.AspNetCore.Components;

namespace JCRazor.盒子;
public partial class LeftRightLayoutBox
{
	#region 内容
	[Parameter]
	public RenderFragment? Head { get; set; }
	[Parameter]
	public RenderFragment? LeftHead { get; set; }
	[Parameter]
	public RenderFragment? MiddleHead { get; set; }
	[Parameter]
	public RenderFragment? RightHead { get; set; }

	[Parameter]
	public RenderFragment? LeftContent { get; set; }
	[Parameter]
	public RenderFragment? MiddleContent { get; set; }
	[Parameter]
	public RenderFragment? RightContent { get; set; }

	[Parameter]
	public RenderFragment? LeftFoot { get; set; }
	[Parameter]
	public RenderFragment? MiddleFoot { get; set; }
	[Parameter]
	public RenderFragment? RightFoot { get; set; }
	[Parameter]
	public RenderFragment? Foot { get; set; }
	#endregion

	#region root 样式
	[Parameter]
	public string Width { get; set; } = "100%";
	[Parameter]
	public string Height { get; set; } = "100%";
	[Parameter]
	public string Overflow { get; set; } = "hidden";
	#endregion

	#region 内容样式
	[Parameter]
	public string LeftContentPadding { get; set; } = "0";
	[Parameter]
	public string MiddleContentPadding { get; set; } = "0";
	[Parameter]
	public string RightContentPadding { get; set; } = "0";
	#endregion
}
