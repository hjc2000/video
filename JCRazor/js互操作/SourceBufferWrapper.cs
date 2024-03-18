using Microsoft.JSInterop;

namespace JCRazor.js互操作;
public class SourceBufferWrapper : IAsyncDisposable, IJSObjectProjection
{
	#region 生命周期
	/// <summary>
	///		
	/// </summary>
	/// <param name="jsrt"></param>
	/// <param name="source_buffer">js 中的 SourceBuffer 对象。</param>
	public SourceBufferWrapper(IJSRuntime jsrt, IJSObjectReference source_buffer)
	{
		_jsop = new JSOp(jsrt);
		_jsm = new JSModule(jsrt, "./_content/JCRazor/SourceBufferWrapper.js");
		_update_end_callback_helper.CallbackAction = () =>
		{
			try
			{
				if (_update_end_signal.CurrentCount >= 1)
				{
					return;
				}

				_update_end_signal.Release();
			}
			catch { }
		};
		_ = InitAsync(source_buffer);
	}

	private async Task InitAsync(IJSObjectReference source_buffer)
	{
		Projection = await _jsm.InvokeAsync<IJSObjectReference>(
			"SourceBufferWrapper.Create",
			source_buffer,
			_update_end_callback_helper.DotNetHelper
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

		await _init_tcs.Task;
		await _jsop.DisposeAsync();
		await Projection.InvokeVoidAsync("Dispose");
		await Projection.DisposeAsync();
		try
		{
			_update_end_signal.Release();
		}
		catch { }
	}
	#endregion

	public IJSObjectReference Projection { get; set; } = default!;
	private JSOp _jsop;
	private JSModule _jsm;
	private TaskCompletionSource _init_tcs = new();
	private CallbackHelper _update_end_callback_helper = new();

	/// <summary>
	///		直接用 jsinterop 更改 SourceBuffer 的函数，结束时都要等待此信号。不是直接用 jsinterop 更改 SourceBuffer
	///		的函数不能等待此信号量，否则会死锁。
	/// </summary>
	private SemaphoreSlim _update_end_signal = new(0, 1);

	/// <summary>
	///		将流中的数据添加到播放缓冲区中。
	/// </summary>
	/// <remarks>
	///		禁忌：不要将很长的流甚至是无限长度的流传给本函数。因为本函数会将流中的所有数据都
	///		复制到 arrayBuffer 中后才会将这个 arrayBuffer 追加到播放缓冲区。
	/// </remarks>
	/// <param name="stream"></param>
	/// <returns></returns>
	public async Task AppendBufferAsync(Stream stream)
	{
		await _init_tcs.Task;
		DotNetStreamReference dotnet_stream_ref = new(stream);
		await Projection.InvokeVoidAsync("AppendBufferByStream", dotnet_stream_ref);
		await _update_end_signal.WaitAsync();
	}

	public async Task AppendBufferAsync(byte[] buf)
	{
		await _init_tcs.Task;
		await Projection.InvokeVoidAsync("AppendBufferByArray", buf);
		await _update_end_signal.WaitAsync();
	}

	/// <summary>
	///		当前缓冲的数据的头部所对应的播放时间。单位：秒。
	/// </summary>
	/// <returns></returns>
	public async Task<double> GetBufferStartTimeAsync()
	{
		await _init_tcs.Task;
		return await Projection.InvokeAsync<double>("BufferStartTime");
	}

	/// <summary>
	///		当前缓冲的数据的尾部所对应的播放时间。单位：秒。
	/// </summary>
	/// <returns></returns>
	public async Task<double> GetBufferEndTimeAsync()
	{
		await _init_tcs.Task;
		return await Projection.InvokeAsync<double>("BufferEndTime");
	}

	/// <summary>
	///		移除指定时间范围的数据。这里的时间是播放时间。
	/// </summary>
	/// <param name="start_time">要移除的视频片段的起始时间。</param>
	/// <param name="end_time">要移除的视频片段的结束时间。</param>
	/// <returns></returns>
	public async Task RemoveDataAsync(double start_time, double end_time)
	{
		await _init_tcs.Task;
		await Projection.InvokeVoidAsync("RemoveData", start_time, end_time);
		await _update_end_signal.WaitAsync();
	}

	/// <summary>
	///		尝试将缓冲区中从头部开始，直到 time 的数据移除。
	/// </summary>
	/// <param name="time"></param>
	/// <returns></returns>
	public async Task TryRemoveUntilAsync(double time)
	{
		// 防止上溢
		double end_time = await GetBufferEndTimeAsync();
		if (time > end_time)
		{
			time = end_time;
		}

		double start_time = await GetBufferStartTimeAsync();
		if (time <= start_time)
		{
			return;
		}

		await _jsop.LogAsync($"移除 {start_time} -- {time}");
		await RemoveDataAsync(start_time, time);
	}
}
