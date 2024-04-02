using JCRazor.js互操作;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace JCRazor.WindowComponents;

public partial class Dialog : IJSObjectProjection, IAsyncDisposable
{
	#region 生命周期
	public Dialog()
	{
		_close_callback_helper.CallbackAction = async () =>
		{
			Console.WriteLine("弹窗关闭");
			await DialogClosedEvent.InvokeAsync();
		};
	}

	protected override async Task OnAfterRenderAsync(bool firstRender)
	{
		await base.OnAfterRenderAsync(firstRender);
		if (!firstRender)
		{
			return;
		}

		_jsm = new JSModule(_jsrt, "./_content/JCRazor/WindowComponents/Dialog.razor.js");

		Projection = await _jsm.InvokeAsync<IJSObjectReference>(
			 "DialogWrapper.create",
			 _dialog_element,
			 _close_callback_helper.DotNetHelper
		);

		_init_tcs.TrySetResult();
	}

	private bool _disposed = false;
	public async ValueTask DisposeAsync()
	{
		if (_disposed)
		{
			return;
		}

		_disposed = true;
		GC.SuppressFinalize(this);

		await _jsm.DisposeAsync();
		await Projection.DisposeAsync();
		await _close_callback_helper.DisposeAsync();
	}
	#endregion

	private JSModule _jsm = default!;
	public IJSObjectReference Projection { get; set; } = default!;
	private TaskCompletionSource _init_tcs = new();
	private ElementReference _dialog_element;
	private CallbackHelper _close_callback_helper = new();

	/// <summary>
	///		对话框关闭后会触发此事件
	/// </summary>
	[Parameter]
	public EventCallback DialogClosedEvent { get; set; }

	[Parameter]
	public RenderFragment? ChildContent { get; set; }

	/// <summary>
	///		以非模态形式显示对话框
	/// </summary>
	/// <returns></returns>
	public async Task ShowAsync()
	{
		await _init_tcs.Task;
		await Projection.InvokeVoidAsync("show");
	}

	/// <summary>
	///		以模态形式显示对话框
	/// </summary>
	/// <returns></returns>
	public async Task ShowModalAsync()
	{
		await _init_tcs.Task;
		await Projection.InvokeVoidAsync("showModal");
	}

	/// <summary>
	///		关闭对话框
	/// </summary>
	/// <returns></returns>
	public async Task CloseAsync()
	{
		await _init_tcs.Task;
		await Projection.InvokeVoidAsync("close");
	}

	/// <summary>
	///		查询当前对话框是否开启。不管是模态的还是非模态的。
	/// </summary>
	/// <returns></returns>
	public async Task<bool> IsOpened()
	{
		await _init_tcs.Task;
		return await Projection.InvokeAsync<bool>("isOpened");
	}

	[Parameter(CaptureUnmatchedValues = true)]
	public Dictionary<string, object>? Attributes { get; set; }
}
