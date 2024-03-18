using Microsoft.AspNetCore.Components;

namespace JCRazor.盒子;
public partial class OutlineTitleBox
{
	[Parameter]
	public RenderFragment? ChildContent { get; set; }
}
