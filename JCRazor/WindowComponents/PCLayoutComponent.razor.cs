using JCRazor.js互操作;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace JCRazor.WindowComponents;

public partial class PCLayoutComponent
{
	protected override async Task OnAfterRenderAsync(bool firstRender)
	{
		if (!firstRender)
		{
			return;
		}

		_jsm = new JSModule(JSRuntime, "./_content/JCRazor/WindowComponents/PCLayoutComponent.razor.js");
		_jsop = new JSOp(JSRuntime);
		_initTcs.TrySetResult();

		// js 模块初始化完毕，可以使用了。
		await SetBodyStyleAsync();

		/* 初始化的时候先获取当前的计算样式的宽度，然后设置一遍，将计算样式应用到内联样式中
		 * 如果不这么做，宽度变化的过渡效果不会生效。想要生效必须初始时有明确设置一个固定宽度。
		 */
		_init_left_menu_width = await GetLeftMenuWidthAsync();
		await SetLeftMenuWidthAsync(_init_left_menu_width);
	}

	private JSModule _jsm = default!;
	private JSOp _jsop = default!;
	private TaskCompletionSource _initTcs = new();

	/// <summary>
	///		左侧边栏初始的宽度
	/// </summary>
	private double _init_left_menu_width = 0;

	/// <summary>
	///		反转侧边栏的隐藏状态。本来出现的，调用后会隐藏。本来隐藏的，调用后会出现。
	/// </summary>
	/// <returns></returns>
	private async Task ToggleHideLeftMenuAsync()
	{
		double width = await GetLeftMenuWidthAsync();
		if (width > 0)
		{
			await SetLeftMenuWidthAsync(0);
		}
		else
		{
			await SetLeftMenuWidthAsync(_init_left_menu_width);
		}
	}

	/// <summary>
	/// 获取侧边栏的宽度
	/// </summary>
	/// <returns></returns>
	public async Task<double> GetLeftMenuWidthAsync()
	{
		await _initTcs.Task;
		string width_string = await _jsop.GetComputedStyle(_left_menu_element, "width");
		width_string = width_string[..(^2)];
		try
		{
			return double.Parse(width_string);
		}
		catch
		{
			return 300;
		}
	}

	/// <summary>
	/// 设置侧边栏的宽度
	/// </summary>
	/// <returns></returns>
	public async Task SetLeftMenuWidthAsync(double width)
	{
		await _initTcs.Task;
		await _jsop.SetElementStyle(_left_menu_element, "width", $"{width}px");
	}

	private async Task SetBodyStyleAsync()
	{
		await _jsm.InvokeVoidAsync("set_body_style");
	}

	private ElementReference _left_menu_element = default!;

	[Parameter]
	public RenderFragment? LeftMenu { get; set; }

	[Parameter]
	public RenderFragment? TopBar { get; set; }

	[Parameter]
	public RenderFragment? Content { get; set; }

	[Parameter]
	public RenderFragment? StateBar { get; set; }

	/// <summary>
	/// 用于隐藏左侧边栏的按钮。如果设置了本参数，则内置的按钮将被禁用，转而使用这个
	/// </summary>
	[Parameter]
	public RenderFragment? HideLeftMenuButton { get; set; }

	[Parameter]
	public string Width { get; set; } = "100vw";
	[Parameter]
	public string Height { get; set; } = "100vh";
}
