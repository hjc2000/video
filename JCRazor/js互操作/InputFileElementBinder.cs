using Microsoft.JSInterop;

namespace JCRazor.js互操作;
public class InputFileElementBinder : IJSObjectProjection, IAsyncDisposable
{
	public InputFileElementBinder(
		IJSRuntime jsrt,
		bool multiple,
		string[]? accepts = null
	)
	{
		_jsrt = jsrt;
		_file_load_callback_helper.CallbackAction = (fileCount) =>
		{
			FileCount = fileCount;
			_loading_file_tcs.TrySetResult();
		};

		_ = InitAsync(multiple, accepts);
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

		await _init_tcs.Task;
		await RemoveAsync();
		await Projection.DisposeAsync();
	}

	private TaskCompletionSource _init_tcs = new();
	private async Task InitAsync(bool multiple, string[]? accepts = null)
	{
		await using JSModule jsm = new(_jsrt, "./_content/JCRazor/InputFileElementBinder.js");
		string? acceptStr = null;
		if (accepts != null)
		{
			acceptStr = string.Join(",", accepts);
		}

		Projection = await jsm.InvokeAsync<IJSObjectReference>(
			"InputFileElementBinder.create",
			acceptStr,
			multiple,
			_file_load_callback_helper.DotNetHelper
		);

		_init_tcs.TrySetResult();
	}

	/// <summary>
	///		input 标签的文件加载完成后将此任务设为完成状态
	/// </summary>
	private TaskCompletionSource _loading_file_tcs = new();

	/// <summary>
	///		回调帮助类对象
	/// </summary>
	private CallbackHelper<int> _file_load_callback_helper = new();

	private IJSRuntime _jsrt = default!;

	/// <summary>
	///		选取了几个文件。在 _file_load_callback_helper 的回调函数中被设置
	/// </summary>
	public int FileCount { get; private set; }

	/// <summary>
	///		触发输入标签的点击事件，打开文件选取窗口让用户选取文件。返回的任务结束后，就可查看 FileCount
	///		属性，看选取了多少个文件。
	/// </summary>
	/// <returns></returns>
	public async Task ClickAsync()
	{
		await _init_tcs.Task;
		await Projection.InvokeVoidAsync("click");
		await _loading_file_tcs.Task;
	}

	/// <summary>
	///		通过 JSReadableStreamBinder 的方式读取文件。
	///		返回的 JSReadableStreamBinder 对象已经被调用了 InitLengthAsync 方法。
	/// </summary>
	/// <param name="fileIndex">要读取第几个文件</param>
	/// <param name="slice_start">从第几个字节开始切片。传入大于 0 的值才会进行切片。</param>
	/// <returns></returns>
	public async Task<JSReadableStreamBinder> OpenReadAsStreamAsync(int fileIndex, long slice_start)
	{
		await _init_tcs.Task;
		IJSObjectReference readable_stream = await Projection.InvokeAsync<IJSObjectReference>(
			"get_file_as_stream",
			fileIndex,
			slice_start
		);
		JSReadableStreamBinder readable_stream_binder = new(_jsrt, readable_stream);
		await readable_stream_binder.InitLengthAsync();
		return readable_stream_binder;
	}

	/// <summary>
	///		从 document 中移除此 input 标签
	/// </summary>
	/// <returns></returns>
	private async Task RemoveAsync()
	{
		await _init_tcs.Task;
		await Projection.InvokeVoidAsync("remove");
	}

	public IJSObjectReference Projection { get; private set; } = default!;
}
