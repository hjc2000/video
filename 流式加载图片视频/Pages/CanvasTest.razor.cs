using JSLib;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace 流式加载图片视频.Pages;
public partial class CanvasTest
{
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
		await _js_canvas.InvokeVoidAsync("play");
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
}
