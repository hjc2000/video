using Microsoft.AspNetCore.Components;

namespace JCRazor.按钮;
public partial class STButton
{
	[Parameter]
	public RenderFragment? ChildContent { get; set; }

	[Parameter(CaptureUnmatchedValues = true)]
	public Dictionary<string, object>? CaptureUnmatchedValues { get; set; }
}
