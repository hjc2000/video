using Microsoft.AspNetCore.Components;

namespace JCRazor.WindowComponents;
public partial class EnsureDialog
{
	private Dialog _ensure_dialog = default!;

	/// <summary>
	///		显示模态对话框
	/// </summary>
	/// <returns></returns>
	public async Task ShowModalAsync()
	{
		await _ensure_dialog.ShowModalAsync();
	}

	public async Task CloseAsync()
	{
		await _ensure_dialog.CloseAsync();
	}

	/// <summary>
	///		设置了此属性将会显示此属性，否则显示 Text。
	/// </summary>
	[Parameter]
	public RenderFragment? ChildContent { get; set; }

	/// <summary>
	///		显示在窗口中的文本，用来向用户询问。
	///		<br/>* 设置了 Content 将会显示 Content，不会显示此属性的文本。
	/// </summary>
	[Parameter]
	public string Text { get; set; } = string.Empty;

	/// <summary>
	///		确保的按钮（确认、不确认按钮）被点击后将触发此回调。传入 true 表示确认，传入 false 表示不确认。
	/// </summary>
	[Parameter]
	public EventCallback<bool> EnsureCallback { get; set; }

	private async Task OnYesAsync()
	{
		await EnsureCallback.InvokeAsync(true);
	}

	private async Task OnNoAsync()
	{
		await EnsureCallback.InvokeAsync(false);
	}
}
