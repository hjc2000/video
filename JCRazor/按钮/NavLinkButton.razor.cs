using Microsoft.AspNetCore.Components;

namespace JCRazor.按钮;
public partial class NavLinkButton
{
	[Parameter]
	public string Url { get; set; } = "./";

	[Parameter]
	public RenderFragment? Left { get; set; }
	[Parameter]
	public RenderFragment? Right { get; set; }
}
