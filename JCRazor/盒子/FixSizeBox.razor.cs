using JCNET.Parse;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace JCRazor.盒子;
/// <summary>
///		* 刚开始宽高设置为 100%，自动调整宽高，第二次渲染完成后会用 js 获取计算样式的宽高，
///		  然后设置为自己的宽高，使宽高变成固定值。<br/>
///		* 注意，第一次渲染完成并不会用 js 获取计算样式的宽高然后设置为固定值。<br/>
/// </summary>
public partial class FixSizeBox
{
	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		_jsop = new JSOp(JSRuntime);
		_init_tcs.TrySetResult();
	}

	private ulong _render_times = 0;
	protected override async Task OnAfterRenderAsync(bool firstRender)
	{
		await base.OnAfterRenderAsync(firstRender);
		_render_times++;
		if (_render_times == 2)
		{
			if (FixWidth)
			{
				await FixWidthAsync();
			}

			if (FixHeight)
			{
				await FixHeightAsync();
			}
		}
	}

	[Parameter]
	public RenderFragment? ChildContent { get; set; }

	[Parameter]
	public bool FixWidth { get; set; } = true;
	[Parameter]
	public bool FixHeight { get; set; } = true;

	[Inject]
	private IJSRuntime JSRuntime { get; set; } = default!;
	private TaskCompletionSource _init_tcs = new();
	private ElementReference _root_element;
	private JSOp _jsop = default!;

	private async Task FixWidthAsync()
	{
		try
		{
			await _init_tcs.Task;
			string width_string = await _jsop.GetComputedStyle(_root_element, "width");
			// 去掉结尾的 px
			width_string = width_string[..(^2)];
			double width = width_string.ToDouble();
			await _jsop.SetElementStyle(_root_element, "width", $"{width}px");
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	private async Task FixHeightAsync()
	{
		try
		{
			await _init_tcs.Task;
			string height_string = await _jsop.GetComputedStyle(_root_element, "height");
			// 去掉结尾的 px
			height_string = height_string[..(^2)];
			double height = height_string.ToDouble();
			await _jsop.SetElementStyle(_root_element, "height", $"{height}px");
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}
}
