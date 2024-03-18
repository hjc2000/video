using Microsoft.AspNetCore.Components;

namespace JCRazor.WindowComponents;
public partial class ProgressBar
{
	[Parameter]
	public double Value { get; set; } = 0;

	[Parameter]
	public double MaxValue { get; set; }

	/// <summary>
	///		进度条左侧的描述信息。用来描述该进度条是干嘛的。
	/// </summary>
	[Parameter]
	public RenderFragment? DescriptionContent { get; set; }

	/// <summary>
	///		进度条右侧的用来指示进度的内容。一般是文本，例如：90% 这样子的文本。
	///		如果没有设置此属性，则默认就是显示百分比。
	/// </summary>
	[Parameter]
	public RenderFragment? ProgressIndication { get; set; }

	[Parameter]
	public bool UseCancelButton { get; set; } = false;

	/// <summary>
	///		当 UseCancelButton 为 true 时，进度条右侧会出现取消按钮。点击取消按钮后会触发此事件。
	/// </summary>
	[Parameter]
	public EventCallback CancelButtonClickEvent { get; set; }

	[Parameter(CaptureUnmatchedValues = true)]
	public Dictionary<string, object>? Attributes { get; set; }

	private async Task OnCancelButtonClick()
	{
		await CancelButtonClickEvent.InvokeAsync();
	}
}
