using JSInteropLib;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace JCRazor.mpegts_player;
public partial class MpegTSPlayer : IJSObjectProjection, IAsyncDisposable
{
	public MpegTSPlayer()
	{
		_frame_drop_log_callback_helper.CallbackAction = async () =>
		{
			try
			{
				await _handle_log_lock.WaitAsync();
				await DestroyPlayerAsync();
				await InitPlayerAsync();
				await LoadAndPlayAsync();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.ToString());
			}
			finally
			{
				_handle_log_lock.Release();
			}
		};

		_source_open_callback_helper.CallbackAction = OnSourceOpenAsync;
	}

	protected override async void OnAfterRender(bool firstRender)
	{
		await base.OnAfterRenderAsync(firstRender);
		if (!firstRender)
		{
			return;
		}

		_jsop = new JSOp(JSRuntime);
		await _jsop.AddScriptAsync("./_content/JCRazor/mpegts.js");
		_jsm = new JSModule(JSRuntime, "./_content/JCRazor/mpegts_player/MpegTSPlayer.razor.js");
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
		await _frame_drop_log_callback_helper.DisposeAsync();
		await _source_open_callback_helper.DisposeAsync();
		if (_jsop is not null)
		{
			await _jsop.DisposeAsync();
		}

		if (Projection is not null)
		{
			await DestroyPlayerAsync();
			await Projection.DisposeAsync();
		}
	}

	private bool _clicked = false;
	private async Task OnClickAsync()
	{
		if (_clicked)
		{
			return;
		}

		_clicked = true;

		await InitPlayerAsync();
		await LoadAndPlayAsync();
	}

	/// <summary>
	///		mpegts 触发源打开事件后就会调用此函数
	/// </summary>
	private async void OnSourceOpenAsync()
	{
		CancellationToken cancellationToken = _cancel_push_ts.Token;

		// 剩余未播放的时间大于此值时将会睡眠。睡眠时间为 sleep_time，单位毫秒。
		double sleep_threshold = 10;
		int sleep_time = 5000;

		/* 剩余时间超过此值时会抛出异常。因为会睡眠，所以剩余时间不应大于 sleep_threshold 太多，
		 * 如果真的很多，说明出错了。这里的单位是秒。剩余时间大于 exception_remain_threshold 秒时将会抛出异常。
		 */
		double exception_remain_threshold = 30;

		/* 不能每次送入 ts 包后都检查当前播放时间和缓冲区末尾时间，这样太耗资源，播放会卡顿。
		 * loop_times 用于对循环次数进行计数，check_remain_time_threshold 是阈值，当 loop_times
		 * 大于等于 check_remain_time_threshold 时就会计算剩余未播放的时间，然后决定应该睡眠多久。
		 * 
		 * 因为一次循环送入一个包，所以 check_remain_time_threshold 的意义也就是每送入多少个 ts 包检查一次
		 * 剩余未播放时间。
		 */
		int loop_times = 0;
		int check_remain_time_threshold = 20;

		try
		{
			// 播放一次
			async Task PlayOnceAsync()
			{
				Stream? ts_stream = await TSStreamSource.GetTSStreamAsync(cancellationToken);
				if (ts_stream is null)
				{
					Console.WriteLine("获取不到 ts 流");
					return;
				}

				byte[] buffer = new byte[188];
				while (!cancellationToken.IsCancellationRequested)
				{
					#region 睡眠机制
					if (_video_source_buffer_wrapper is not null && loop_times >= check_remain_time_threshold)
					{
						loop_times = 0;
						double remain_time = await _video_source_buffer_wrapper.GetBufferEndTimeAsync() - await GetCurrentTimeAsync();
						if (remain_time > exception_remain_threshold)
						{
							await DestroyPlayerAsync();
							await InitPlayerAsync();
							await LoadAndPlayAsync();
							throw new Exception(
								 $"剩余未播放时间异常的大。因为大于 {sleep_threshold} 秒就开始不送入数据，一直睡眠了，" +
								 $"所以大于 {exception_remain_threshold} 秒绝对有鬼"
							);
						}

						if (remain_time > sleep_threshold)
						{
							await _jsop.LogAsync("睡眠。");
							await _jsop.LogAsync($"剩余未播放的时间：{remain_time}。");
							await _jsop.LogAsync($"当前时间：{await GetCurrentTimeAsync()}。");
							await _jsop.LogAsync();
							await Task.Delay(sleep_time);
							continue;
						}
					}
					else if (_audio_source_buffer_wrapper is not null && loop_times >= check_remain_time_threshold)
					{
						loop_times = 0;
						double remain_time = await _audio_source_buffer_wrapper.GetBufferEndTimeAsync() - await GetCurrentTimeAsync();
						if (remain_time > exception_remain_threshold)
						{
							await DestroyPlayerAsync();
							await InitPlayerAsync();
							await LoadAndPlayAsync();
							throw new Exception(
								 $"剩余未播放时间异常的大。因为大于 {sleep_threshold} 秒就开始不送入数据，一直睡眠了，" +
								 $"所以大于 {exception_remain_threshold} 秒绝对有鬼"
							);
						}

						if (remain_time > sleep_threshold)
						{
							await _jsop.LogAsync("睡眠。");
							await _jsop.LogAsync($"剩余未播放的时间：{remain_time}。");
							await _jsop.LogAsync($"当前时间：{await GetCurrentTimeAsync()}。");
							await _jsop.LogAsync();
							await Task.Delay(sleep_time);
							continue;
						}
					}
					else
					{
						loop_times++;
					}
					#endregion

					int have_read = await ts_stream.ReadAtLeastAsync(
						buffer,
						buffer.Length,
						false,
						cancellationToken
					);

					if (have_read < buffer.Length)
					{
						// 流结束
						break;
					}

					await SendBufferToLoaderAsync(buffer, have_read);
				}
			}

			while (!cancellationToken.IsCancellationRequested)
			{
				await PlayOnceAsync();
			}
		}
		catch (Exception e)
		{
			await _jsop.LogAsync(e.ToString());
		}
	}

	/// <summary>
	///		检查是否支持。
	/// </summary>
	/// <returns></returns>
	public async Task<bool> IsSupportAsync()
	{
		try
		{
			await _init_tcs.Task;
			return await _jsm.InvokeAsync<bool>("MpegTSPlayer.is_support");
		}
		catch
		{
			return false;
		}
	}

	[Parameter]
	public ITSStreamSource TSStreamSource { get; set; } = default!;
	[Parameter]
	public bool Controls { get; set; } = true;

	private MediaDataSource MediaDataSource { get; set; } = new MediaDataSource();
	private Config Config { get; set; } = new Config();

	/// <summary>
	///		初始化播放器。
	/// </summary>
	/// <param name="data_source"></param>
	/// <param name="config"></param>
	/// <returns></returns>
	private async Task InitPlayerAsync()
	{
		try
		{
			await _init_tcs.Task;
			Projection = await _jsm.InvokeAsync<IJSObjectReference>(
				"MpegTSPlayer.Create",
				MediaDataSource,
				Config,
				_video_element,
				_frame_drop_log_callback_helper.DotNetHelper,
				_source_open_callback_helper.DotNetHelper
			);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	private CancellationTokenSource _cancel_push_ts = new();

	/// <summary>
	///		销毁播放器
	/// </summary>
	/// <returns></returns>
	private async Task DestroyPlayerAsync()
	{
		try
		{
			await _init_tcs.Task;
			_cancel_push_ts.Cancel();
			_cancel_push_ts = new CancellationTokenSource();
			_video_source_buffer_wrapper = null;
			_audio_source_buffer_wrapper = null;
			await Projection.InvokeVoidAsync("Dispose");
			await Projection.DisposeAsync();
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	public async Task LoadAndPlayAsync()
	{
		try
		{
			await _init_tcs.Task;
			await Projection.InvokeVoidAsync("Load");
			await Projection.InvokeVoidAsync("Play");
		}
		catch (Exception e)
		{
			await _jsop.LogAsync(e.ToString());
		}
	}

	private async Task SendBufferToLoaderAsync(byte[] buffer, int length)
	{
		await Projection.InvokeVoidAsync("SendBufferToLoader", buffer, length);
	}

	private async Task<IJSObjectReference?> GetVideoSourceBufferAsync()
	{
		try
		{
			await _init_tcs.Task;
			return await Projection.InvokeAsync<IJSObjectReference>("GetVideoSourceBuffer");
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
			return null;
		}
	}

	private async Task<IJSObjectReference?> GetAudioSourceBufferAsync()
	{
		try
		{
			await _init_tcs.Task;
			return await Projection.InvokeAsync<IJSObjectReference>("GetAudioSourceBuffer");
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
			return null;
		}
	}

	public async Task<double> GetCurrentTimeAsync()
	{
		await _init_tcs.Task;
		return await Projection.InvokeAsync<double>("CurrentTime");
	}

	private TaskCompletionSource _init_tcs = new();
	public IJSObjectReference Projection { get; set; } = default!;
	private JSOp _jsop = default!;
	private JSModule _jsm = default!;
	private ElementReference _video_element;
	private CallbackHelper _frame_drop_log_callback_helper = new();
	private CallbackHelper _source_open_callback_helper = new();
	private SemaphoreSlim _handle_log_lock = new(1, 1);
	private SourceBufferWrapper? _video_source_buffer_wrapper = null;
	private SourceBufferWrapper? _audio_source_buffer_wrapper = null;

	private async Task OnPlayingAsync()
	{
		IJSObjectReference? video_source_buffer = await GetVideoSourceBufferAsync();
		if (video_source_buffer is not null)
		{
			_video_source_buffer_wrapper = new SourceBufferWrapper(JSRuntime, video_source_buffer);
		}

		IJSObjectReference? audio_source_buffer = await GetAudioSourceBufferAsync();
		if (audio_source_buffer is not null)
		{
			_audio_source_buffer_wrapper = new SourceBufferWrapper(JSRuntime, audio_source_buffer);
		}
	}
}
