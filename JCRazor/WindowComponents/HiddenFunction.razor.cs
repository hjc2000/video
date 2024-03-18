using JCRazor.js互操作;
using Microsoft.JSInterop;

namespace JCRazor.WindowComponents;
public partial class HiddenFunction : IAsyncDisposable
{
	#region 生命周期
	public HiddenFunction()
	{
		_show_hidden_function_callback_helper.CallbackAction = () =>
		{
			ShouldShowHiddenFunction = true;
		};
	}

	protected override async Task OnInitializedAsync()
	{
		_jsm = new(_jsrt, "./_content/JCRazor/WindowComponents/HiddenFunction.razor.js");
		_js_HiddenFunction = await _jsm.InvokeAsync<IJSObjectReference>("HiddenFunction.create", _show_hidden_function_callback_helper.DotNetHelper);
	}

	private bool _disposed = false;
	public async ValueTask DisposeAsync()
	{
		if (_disposed)
		{ return; }

		_disposed = true;
		GC.SuppressFinalize(this);

		if (_jsm != null)
		{
			await _jsm.DisposeAsync();
		}

		if (_js_HiddenFunction != null)
		{
			await _js_HiddenFunction.DisposeAsync();
		}
	}
	#endregion

	private JSModule _jsm = default!;

	/// <summary>
	/// js 的 HiddenFunction 类的对象的引用
	/// </summary>
	private IJSObjectReference _js_HiddenFunction = default!;
	private CallbackHelper _show_hidden_function_callback_helper = new();

	private static bool _should_show_hidden_function = false;
	public static bool ShouldShowHiddenFunction
	{
		get
		{
			return _should_show_hidden_function;
		}
		set
		{
			_should_show_hidden_function = value;
			Console.WriteLine("显示隐藏功能");
			ShouldShowHiddenFunctionChanged?.Invoke();
		}
	}

	/// <summary>
	/// HiddenFunction 属性改变事件
	/// </summary>
	public static event Action? ShouldShowHiddenFunctionChanged;
}
