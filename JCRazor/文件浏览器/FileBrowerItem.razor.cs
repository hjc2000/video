using JCNET.容器;
using JSInteropLib;
using Microsoft.AspNetCore.Components;
using Microsoft.AspNetCore.Components.Web;
using Microsoft.JSInterop;

namespace JCRazor.文件浏览器;
public partial class FileBrowerItem : IAsyncDisposable
{
	protected override void OnInitialized()
	{
		base.OnInitialized();
		SelecteEndEvent += OnSelectEndAsync;
		_jsop = new JSOp(JSRuntime);
		FileBrower.RenameEvent += OnRenameEventAsync;
		_init_tcs.TrySetResult();
	}

	protected override void OnAfterRender(bool firstRender)
	{
		base.OnAfterRender(firstRender);
		_render_tcs.TrySetResult();
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

		await Task.CompletedTask;
		_init_tcs.TrySetCanceled();
		_render_tcs.TrySetCanceled();
		SelecteEndEvent -= OnSelectEndAsync;
		FileBrower.RenameEvent -= OnRenameEventAsync;
	}

	private TaskCompletionSource _render_tcs = new();
	private TaskCompletionSource _init_tcs = new();
	private JSOp _jsop = default!;

	[Parameter]
	public IDirectoryInfos? Directory { get; set; } = null;

	[Parameter]
	public IFileInfos? File { get; set; } = null;

	[Parameter]
	public int RenderIndex { get; set; } = -1;

	[Parameter]
	public EventCallback<FileBrowerItem> DoubleClickEvent { get; set; }

	/// <summary>
	///		子组件更新了一些状态，需要反馈到父组件，就会触发此事件。
	/// </summary>
	[Parameter]
	public EventCallback StateChangeEvent { get; set; }

	[Parameter]
	public ConcurrentHashSet<IFileInfos> SelectedFiles { get; set; } = default!;
	[Parameter]
	public ConcurrentHashSet<IDirectoryInfos> SelectedDirectories { get; set; } = default!;

	[Parameter]
	public EventCallback<RenameEventArgs> RenameEvent { get; set; }

	private async Task OnClickAsync(MouseEventArgs e)
	{
		try
		{
			if (e.ShiftKey && e.Button == 0)
			{
				await OnShiftLeftClickAsync();
				return;
			}

			// 仅仅单击，没有按下 shift 键。
			SelectedDirectories.Clear();
			SelectedFiles.Clear();

			_select_start_index = RenderIndex;
			if (Directory is not null)
			{
				SelectedDirectories.Add(Directory);
			}
			else if (File is not null)
			{
				SelectedFiles.Add(File);
			}

			await StateChangeEvent.InvokeAsync();
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	private static int _select_start_index = -1;
	private static int _select_end_index = -1;

	private async Task OnShiftLeftClickAsync()
	{
		try
		{
			await Task.CompletedTask;
			_select_end_index = RenderIndex;
			if (_select_start_index < 0 || _select_end_index < 0 || _select_start_index == _select_end_index)
			{
				return;
			}

			SelectedDirectories.Clear();
			SelectedFiles.Clear();
			int min = Math.Min(_select_start_index, _select_end_index);
			int max = Math.Max(_select_start_index, _select_end_index);
			_select_start_index = min;
			_select_end_index = max;
			SelecteEndEvent?.Invoke();
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	private static event Action? SelecteEndEvent;

	private async void OnSelectEndAsync()
	{
		if (RenderIndex >= _select_start_index && RenderIndex <= _select_end_index)
		{
			if (Directory is not null)
			{
				SelectedDirectories.Add(Directory);
			}
			else if (File is not null)
			{
				SelectedFiles.Add(File);
			}

			await StateChangeEvent.InvokeAsync();
		}
	}

	/// <summary>
	///		双击事件
	/// </summary>
	/// <returns></returns>
	private async Task OnDoubleClickAsync()
	{
		await DoubleClickEvent.InvokeAsync(this);
	}

	private bool _editting_name = false;
	private ElementReference _dir_name_input_element;
	private ElementReference _dir_name_span_element;
	private ElementReference _file_name_input_element;
	private ElementReference _file_name_span_element;

	[Inject]
	public IJSRuntime JSRuntime { get; set; } = default!;

	private async void OnRenameEventAsync(string name)
	{
		string this_item_name;
		if (Directory is not null)
		{
			this_item_name = Directory.Name;
		}
		else if (File is not null)
		{
			this_item_name = File.Name;
		}
		else
		{
			return;
		}

		if (this_item_name != name)
		{
			return;
		}

		_editting_name = true;

		try
		{
			await _init_tcs.Task;
			_render_tcs = new TaskCompletionSource();
			await _render_tcs.Task;
			StateHasChanged();

			if (File is not null)
			{
				await _jsop.FocusOnInputElement(_file_name_input_element);
			}
			else if (Directory is not null)
			{
				await _jsop.FocusOnInputElement(_dir_name_input_element);
			}
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	private async Task FocusOnSpanAsync()
	{
		_render_tcs = new TaskCompletionSource();
		StateHasChanged();
		await _render_tcs.Task;

		if (File is not null)
		{
			await _jsop.FocusOnElement(_file_name_span_element);
		}
		else if (Directory is not null)
		{
			await _jsop.FocusOnElement(_dir_name_span_element);
		}
	}

	private async Task OnEnterKeyPress(KeyboardEventArgs e)
	{
		try
		{
			if (e.Code != "Enter" && e.Key != "Enter")
			{
				return;
			}

			Console.WriteLine("回车键被按下");
			_editting_name = false;
			await FocusOnSpanAsync();
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	private async Task OnNameChangeAsync(string name)
	{
		Console.WriteLine("名称被更改");
		_editting_name = false;
		await FocusOnSpanAsync();

		RenameEventArgs args = new()
		{
			OldFullPath = FullPath,
			NewFullPath = $"{BasePath}{name}"
		};

		if (Directory is not null)
		{
			args.NewFullPath += "/";
		}

		await RenameEvent.InvokeAsync(args);
	}

	private async Task OnFocusoutAsync()
	{
		Console.WriteLine("失去焦点");
		_editting_name = false;
		await FocusOnSpanAsync();
	}

	private bool Selected
	{
		get
		{
			if (Directory is not null)
			{
				return SelectedDirectories.Contains(Directory);
			}
			else if (File is not null)
			{
				return SelectedFiles.Contains(File);
			}

			return false;
		}
	}
	private async Task OnSelectedChange(bool value)
	{
		if (Directory is not null)
		{
			if (value)
			{
				SelectedDirectories.Add(Directory);
			}
			else
			{
				SelectedDirectories.Remove(Directory);
			}
		}
		else if (File is not null)
		{
			if (value)
			{
				SelectedFiles.Add(File);
			}
			else
			{
				SelectedFiles.Remove(File);
			}
		}

		await StateChangeEvent.InvokeAsync();
	}

	private string BasePath
	{
		get
		{
			if (File is not null)
			{
				return File.BasePath;
			}
			else if (Directory is not null)
			{
				return Directory.BasePath;
			}
			else
			{
				throw new Exception($"父组件必须传入 {nameof(File)} 参数或 {nameof(Directory)} 参数中的任意一个");
			}
		}
	}

	public string Name
	{
		get
		{
			if (File is not null)
			{
				return File.Name;
			}
			else if (Directory is not null)
			{
				return Directory.Name;
			}
			else
			{
				throw new Exception($"父组件必须传入 {nameof(File)} 参数或 {nameof(Directory)} 参数中的任意一个");
			}
		}
	}

	private string FullPath
	{
		get
		{
			if (File is not null)
			{
				return File.FullPath;
			}
			else if (Directory is not null)
			{
				return Directory.FullPath;
			}
			else
			{
				throw new Exception($"父组件必须传入 {nameof(File)} 参数或 {nameof(Directory)} 参数中的任意一个");
			}
		}
	}
}

public struct RenameEventArgs
{
	public string OldFullPath { get; set; }
	public string NewFullPath { get; set; }
}