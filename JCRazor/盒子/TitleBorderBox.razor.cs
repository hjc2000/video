using Microsoft.AspNetCore.Components;

namespace JCRazor.盒子;
public partial class TitleBorderBox
{
	[Parameter]
	public RenderFragment? Title { get; set; }

	[Parameter]
	public RenderFragment? Content { get; set; }
}
