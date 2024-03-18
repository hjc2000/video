using Microsoft.JSInterop;
using System.Diagnostics.CodeAnalysis;

namespace JCRazor.js互操作;

public class JSModule : IJSObjectReference
{
	#region 生命周期
	public JSModule(IJSRuntime jsrt, string js_file_path)
	{
		Init(jsrt, js_file_path);
	}

	private TaskCompletionSource _initTcs = new();

	private async void Init(IJSRuntime jsrt, string js_file_path)
	{
		Module = await jsrt.InvokeAsync<IJSObjectReference>("import", js_file_path);
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

		if (Module != null)
		{
			await Module.DisposeAsync();
		}
	}
	#endregion

	/// <summary>
	/// 用户构建此类对象时导入的自定义模块
	/// </summary>
	public IJSObjectReference Module { get; private set; } = default!;

	#region IJSObjectReference 接口实现
	/// <summary>
	/// 如果感觉不对劲，记得 using Microsoft.JSInterop;
	/// </summary>
	/// <typeparam name="TValue"></typeparam>
	/// <param name="identifier"></param>
	/// <param name="args"></param>
	/// <returns></returns>
	public async ValueTask<TValue> InvokeAsync<[DynamicallyAccessedMembers(DynamicallyAccessedMemberTypes.PublicConstructors | DynamicallyAccessedMemberTypes.PublicFields | DynamicallyAccessedMemberTypes.PublicProperties)] TValue>(string identifier, object?[]? args)
	{
		await _initTcs.Task;
		return await Module.InvokeAsync<TValue>(identifier, args);
	}

	/// <summary>
	/// 如果感觉不对劲，记得 using Microsoft.JSInterop;
	/// </summary>
	/// <typeparam name="TValue"></typeparam>
	/// <param name="identifier"></param>
	/// <param name="cancellationToken"></param>
	/// <param name="args"></param>
	/// <returns></returns>
	public async ValueTask<TValue> InvokeAsync<[DynamicallyAccessedMembers(DynamicallyAccessedMemberTypes.PublicConstructors | DynamicallyAccessedMemberTypes.PublicFields | DynamicallyAccessedMemberTypes.PublicProperties)] TValue>(string identifier, CancellationToken cancellationToken, object?[]? args)
	{
		await _initTcs.Task;
		return await Module.InvokeAsync<TValue>(identifier, cancellationToken, args);
	}
	#endregion
}