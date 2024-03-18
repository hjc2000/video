using System.Runtime.InteropServices;

namespace PInvokeLib;
public partial class SDL_EventGetterHelper
{
	[LibraryImport("PInvokeNativeHelper", EntryPoint = "CreateSDL_EventGetter")]
	private static unsafe partial nuint CreateSDL_EventGetter();

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "FreeSDL_EventGetter")]
	private static unsafe partial void FreeSDL_EventGetter(nuint native_event_getter);

	[LibraryImport("PInvokeNativeHelper", EntryPoint = "SDL_EventGetter_PollQuitEvent")]
	private static unsafe partial byte SDL_EventGetter_PollQuitEvent();
}

public partial class SDL_EventGetterHelper : IDisposable
{
	public SDL_EventGetterHelper()
	{
		_native_event_getter = CreateSDL_EventGetter();
	}

	~SDL_EventGetterHelper()
	{
		Dispose();
	}

	private bool _disposed = false;
	public void Dispose()
	{
		if (_disposed)
		{
			return;
		}

		_disposed = true;
		GC.SuppressFinalize(this);

		FreeSDL_EventGetter(_native_event_getter);
	}

	private nuint _native_event_getter;

	/// <summary>
	///		需要不断拉取事件 SDL2 窗口才不会卡住。两次拉取之间要快速，不要执行耗时任务。
	/// </summary>
	/// <returns></returns>
	public bool PollQuitEvent()
	{
		byte ret = SDL_EventGetter_PollQuitEvent();
		return ret != 0;
	}
}
