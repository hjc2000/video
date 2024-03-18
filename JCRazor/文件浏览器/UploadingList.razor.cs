using JCNET.容器;
using Microsoft.AspNetCore.Components;

namespace JCRazor.文件浏览器;
public partial class UploadingList
{
	[Parameter]
	public SafeList<UploadingState> UploadingStateList { get; set; } = [];

	private static async Task OnCancelDownloading(UploadingState state)
	{
		await Task.CompletedTask;
		state.CancelDownloading.Cancel();
	}
}

public class UploadingState
{
	public string FileName { get; set; } = string.Empty;
	public double Progress { get; set; } = 0;
	public string? FailureReason { get; set; } = string.Empty;

	/// <summary>
	///		取消下载
	/// </summary>
	public CancellationTokenSource CancelDownloading { get; set; } = new CancellationTokenSource();
}