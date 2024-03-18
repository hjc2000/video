using JCRazor.js互操作;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace JCRazor.WindowComponents;

public partial class Dialog : IJSObjectProjection, IAsyncDisposable
{
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
		Projection = await _jsm.InvokeAsync<IJSObjectReference>("DialogWrapper.create", _dialog_element, _close_callback_helper.DotNetHelper);
		_initTcs.TrySetResult();
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

	private JSModule _jsm = default!;
	public IJSObjectReference Projection { get; set; } = default!;
	private TaskCompletionSource _initTcs = new();
	private ElementReference _dialog_element;
	private CallbackHelper _close_callback_helper = new();

	/// <summary>
	///		对话框关闭后会触发此事件
	/// </summary>
	[Parameter]
	public EventCallback DialogClosedEvent { get; set; }

	[Parameter]
	public RenderFragment? ChildContent { get; set; }

	public async Task ShowAsync()
	{
		await _initTcs.Task;
		await Projection.InvokeVoidAsync("show");
	}

	/// <summary>
	///		显示对话框
	/// </summary>
	/// <returns></returns>
	public async Task ShowModalAsync()
	{
		await _initTcs.Task;
		await Projection.InvokeVoidAsync("showModal");
	}

	/// <summary>
	///		关闭对话框
	/// </summary>
	/// <returns></returns>
	public async Task CloseAsync()
	{
		await _initTcs.Task;
		await Projection.InvokeVoidAsync("close");
	}

	public async Task<bool> IsOpened()
	{
		await _initTcs.Task;
		return await Projection.InvokeAsync<bool>("isOpened");
	}

	[Parameter(CaptureUnmatchedValues = true)]
	public Dictionary<string, object>? Attributes { get; set; }
}
