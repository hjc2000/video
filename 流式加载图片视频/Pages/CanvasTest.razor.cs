using JSLib;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace 流式加载图片视频.Pages;
public partial class CanvasTest
{
	public CanvasTest()
	{
		_buffer = new byte[_width * _height * 4];
		for (int i = 0; i < _buffer.Length; i += 4)
		{
			_buffer[i] = 0;
			_buffer[i + 1] = 0;
			_buffer[i + 2] = 0;
			_buffer[i + 3] = 255;
		}
	}

	protected override async Task OnInitializedAsync()
	{
		_jsop = await JSOp.CreateAsync(JS);
		_jsm = await JSModule.CreateAsync(JS, "./Pages/CanvasTest.razor.js");
		_js_canvas = await _jsm.InvokeAsync<IJSObjectReference>("Canvas.create", _canvas, "2d");
		_initTcs.SetResult();
	}

	private async Task OnClick()
	{
		await _initTcs.Task;
		int barWidth = 100;
		int left = -barWidth;

		CancellationTokenSource cts = new();
		TimerLib.ScheduledTasks.ExecuteUntilCancel((int)(1.0 / 20 * 1000), async () =>
		{
			for (int i = 0; i < _buffer.Length; i += 4)
			{
				int col = (i + 1) / 4 % _width;
				if (col > left && col <= left + barWidth)
				{
					_buffer[i] = 0;
					_buffer[i + 1] = 100;
					_buffer[i + 2] = 0;
				}
				else
				{
					_buffer[i] = 255;
					_buffer[i + 1] = 255;
					_buffer[i + 2] = 255;
				}

				_buffer[i + 3] = 255;
			}

			if (left < _width)
			{
				left++;
			}
			else
			{
				left = -barWidth;
			}

			await _js_canvas.InvokeVoidAsync("putUint8Buffer", _buffer);
		}, cts.Token);
	}

	private int _width = 1280;
	private int _height = 720;

	[Inject]
	private IJSRuntime JS { get; set; } = default!;
	private JSOp _jsop = default!;
	private JSModule _jsm = default!;
	private TaskCompletionSource _initTcs = new();
	private ElementReference _canvas = default!;
	private IJSObjectReference _js_canvas = default!;
	private byte[] _buffer;
}
