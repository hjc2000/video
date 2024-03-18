using JCNET;
using JCNET.容器;
using JCRazor.js互操作;
using Microsoft.AspNetCore.Components;
using Microsoft.AspNetCore.Components.Web;
using Microsoft.JSInterop;

namespace JCRazor.文件浏览器;

public partial class FileBrower : IAsyncDisposable
{
	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		await ChangeDirectoryAsync(CurrentPath);
		_jsop = new JSOp(JSRuntime);
		UploadingStateChangeCallback += OnUploadingStateChange;
		_init_tcs.TrySetResult();
	}

	private bool _disposed = false;
	public async ValueTask DisposeAsync()
	{
		await ValueTask.CompletedTask;
		if (_disposed)
		{
			return;
		}

		_disposed = true;
		GC.SuppressFinalize(this);

		UploadingStateChangeCallback -= OnUploadingStateChange;
	}

	private TaskCompletionSource _init_tcs = new();

	[Parameter]
	public IFileManager FileManager { get; set; } = default!;

	[Inject]
	public IJSRuntime JSRuntime { get; set; } = default!;
	private JSOp _jsop = default!;

	#region 路径
	private static object _current_path_lock = new();
	private static string _current_path = "/";
	private static string CurrentPath
	{
		get
		{
			lock (_current_path_lock)
			{
				if (_current_path.EndsWith("//"))
				{
					_current_path = _current_path[..^1];
				}

				if (!_current_path.EndsWith('/'))
				{
					_current_path += "/";
				}

				return _current_path;
			}
		}
		set
		{
			lock (_current_path_lock)
			{
				_current_path = value;
			}
		}
	}

	private static History<string> PathHistory { get; set; } = new History<string>(10);

	/// <summary>
	///		储存 CurrentPath 各级路径的数组。CurrentPath 属性更新时 SplitedPathArray 会被更新。
	/// </summary>
	private static string[] SplitedPathArray
	{
		get
		{
			string[] arr = CurrentPath.Split('/', StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries);
			return arr;
		}
	}

	/// <summary>
	///		导航栏上的路径按钮被点击了。
	/// </summary>
	/// <param name="index">用来索引 SplitedPathArray，表示是哪一级的路径被点击了。传入负数表示根路径</param>
	/// <returns></returns>
	private async Task OnPathButtonClickAsync(int index)
	{
		if (index < 0 && CurrentPath == "/")
		{
			// 小于 0 表示要切换到根目录，而当前目录就是根目录
			return;
		}

		if (index >= SplitedPathArray.Length - 1)
		{
			// 点击最后一个路径按钮表示保持当前路径不变
			return;
		}

		PathHistory.Add(CurrentPath);
		string path = string.Join('/', SplitedPathArray, 0, index + 1);
		await ChangeDirectoryAsync($"/{path}");
	}
	#endregion

	private SemaphoreSlim _change_directory_lock = new(1, 1);
	/// <summary>
	///		切换路径
	/// </summary>
	/// <param name="dst_full_path">要切换到的完整的路径。</param>
	/// <returns></returns>
	private async Task ChangeDirectoryAsync(string dst_full_path)
	{
		try
		{
			using LockGuard l = new(_change_directory_lock);
			await l.WaitAsync();
			DirectoryList.Clear();
			FileList.Clear();
			SelectedFiles.Clear();
			SelectedDirectories.Clear();

			CurrentPath = dst_full_path;
			Console.WriteLine($"cd {CurrentPath}");
			ChangeDirectoryResult result = await FileManager.ChangeDirecoryAsync(CurrentPath);

			foreach (IDirectoryInfos dir in result.DirectoryList)
			{
				DirectoryList.Add(dir);
			}

			foreach (IFileInfos file in result.FileList)
			{
				FileList.Add(file);
			}
		}
		catch (AlertAbleException e)
		{
			await _init_tcs.Task;
			Console.WriteLine(e);
			await _jsop.AlertAsync(e.Message);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
		finally
		{
			StateHasChanged();
		}
	}

	private SafeList<IDirectoryInfos> DirectoryList { get; set; } = [];
	private SafeList<IFileInfos> FileList { get; set; } = [];

	public ConcurrentHashSet<IFileInfos> SelectedFiles { get; set; } = [];
	public ConcurrentHashSet<IDirectoryInfos> SelectedDirectories { get; set; } = [];

	private async Task AlertAsync(string message)
	{
		try
		{
			await _init_tcs.Task;
			await _jsop.AlertAsync(message);
		}
		catch (Exception e)
		{
			Console.WriteLine(e);
		}
	}

	/// <summary>
	///		文件或文件夹被双击
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	private async Task OnItemDoubleClickAsync(FileBrowerItem item)
	{
		if (item.Directory is not null)
		{
			// 目录被双击
			Console.WriteLine(item.Directory.Name);
			PathHistory.Add(CurrentPath);
			await ChangeDirectoryAsync(item.Directory.FullPath);
		}
		else if (item.File is not null)
		{
			// 文件被双击
			Console.WriteLine(item.File.Name);
			await FileManager.DownLoadFileAsync(item.File.FullPath);
		}
	}

	private async Task OnRenameAsync(RenameEventArgs e)
	{
		try
		{
			Console.WriteLine($"重命名。旧的完整路径：{e.OldFullPath}，新的完整路径：{e.NewFullPath}");
			await FileManager.MoveAsync(e.OldFullPath, e.NewFullPath, false);
			await ChangeDirectoryAsync(CurrentPath);
		}
		catch (AlertAbleException ex)
		{
			Console.WriteLine(ex);
			await AlertAsync(ex.Message);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	private void CopySelectedItemsToItemOperator()
	{
		_item_operator.SelectedDirectories.Clear();
		_item_operator.SelectedFiles.Clear();
		foreach (IDirectoryInfos directory in SelectedDirectories)
		{
			_item_operator.SelectedDirectories.Add(directory);
		}

		foreach (IFileInfos file in SelectedFiles)
		{
			_item_operator.SelectedFiles.Add(file);
		}
	}

	private ItemOperator _item_operator = default!;
	private async Task DeleteSelectedItemsAsync()
	{
		if (SelectedDirectories.Count + SelectedFiles.Count == 0)
		{
			return;
		}

		CopySelectedItemsToItemOperator();
		await _item_operator.DeleteAsync();
		await ChangeDirectoryAsync(CurrentPath);
	}

	/// <summary>
	///		选择移动模式
	/// </summary>
	/// <returns></returns>
	private void SelectMoveMode()
	{
		_move_copy_mode = MoveCopyMode.Move;
		CopySelectedItemsToItemOperator();
	}

	private void SelectCopyMode()
	{
		_move_copy_mode = MoveCopyMode.Copy;
		CopySelectedItemsToItemOperator();
	}

	private enum MoveCopyMode
	{
		Unset,
		Move,
		Copy
	}

	private MoveCopyMode _move_copy_mode = MoveCopyMode.Unset;

	private bool DisablePasteButton
	{
		get
		{
			if (_item_operator is null)
			{
				return true;
			}

			return _item_operator.SelectedFiles.Count + _item_operator.SelectedDirectories.Count == 0;
		}
	}

	/// <summary>
	///		粘贴
	/// </summary>
	/// <returns></returns>
	private async Task PasteItemsAsync()
	{
		if (_move_copy_mode == MoveCopyMode.Move)
		{
			await _item_operator.MoveToAsync(CurrentPath);
			await ChangeDirectoryAsync(CurrentPath);
		}
		else if (_move_copy_mode == MoveCopyMode.Copy)
		{
			await _item_operator.CopyToAsync(CurrentPath);
			await ChangeDirectoryAsync(CurrentPath);
		}

		_move_copy_mode = MoveCopyMode.Unset;
	}

	/// <summary>
	///		返回上一次所在的目录
	/// </summary>
	/// <returns></returns>
	private async Task OnBackClickAsync()
	{
		string old_current_path = CurrentPath;
		IEnumerable<IDirectoryInfos> query_dir = from dir in DirectoryList
												 where dir.FullPath == old_current_path
												 select dir;

		string? dst_path = PathHistory.GetLast();
		if (dst_path is null)
		{
			return;
		}

		await ChangeDirectoryAsync(dst_path);
		lock (DirectoryList)
		{
			IDirectoryInfos? dir_should_be_focused = query_dir.FirstOrDefault();
			if (dir_should_be_focused is not null && dir_should_be_focused.FullPath != "/")
			{
				SelectedDirectories.Add(dir_should_be_focused);
			}
		}
	}

	#region 上传文件
	/// <summary>
	///		上传文件
	/// </summary>
	/// <returns></returns>
	private async Task UploadFilesAsync()
	{
		try
		{
			InputFileElementBinder input_element = new(JSRuntime, true);
			await input_element.ClickAsync();
			for (int i = 0; i < input_element.FileCount; i++)
			{
				JSReadableStreamBinder stream = await input_element.OpenReadAsStreamAsync(i, 0);
				_ = UploadOneStreamAsync(stream);
			}
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	private async void OnUploadingStateChange(bool change_dir)
	{
		if (change_dir && UploadingStateList.Count == 0)
		{
			await ChangeDirectoryAsync(CurrentPath);
			return;
		}

		StateHasChanged();
	}

	private static SafeList<UploadingState> UploadingStateList { get; set; } = [];
	private static event Action<bool>? UploadingStateChangeCallback;

	private async Task UploadOneStreamAsync(JSReadableStreamBinder stream)
	{
		UploadingState uploading_state = new()
		{
			FileName = await stream.GetFileNameAsync()
		};
		UploadingStateList.Add(uploading_state);
		UploadingStateChangeCallback?.Invoke(false);
		stream.PositionChangedEvent += (PositionChangeEventArgs e) =>
		{
			uploading_state.Progress = (double)e.Position / e.Length;
			UploadingStateChangeCallback?.Invoke(false);
		};

		try
		{
			string full_path = $"{CurrentPath}{uploading_state.FileName}";
			await FileManager.UploadFileAsync(full_path, stream, false, uploading_state.CancelDownloading.Token);
		}
		catch (TaskCanceledException)
		{
			Console.WriteLine("取消上传");
		}
		catch (AlertAbleException e)
		{
			await _init_tcs.Task;
			uploading_state.FailureReason = e.Message;
			UploadingStateChangeCallback?.Invoke(false);
			await Task.Delay(10000);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
		finally
		{
			UploadingStateList.Remove(uploading_state);
			UploadingStateChangeCallback?.Invoke(true);
		}
	}
	#endregion

	#region 创建目录
	private bool _disable_create_directory_button = false;
	private string NewDirectoryName { get; set; } = string.Empty;

	private async Task CreateDirectoryAsync()
	{
		try
		{
			if (NewDirectoryName == string.Empty)
			{
				return;
			}

			_disable_create_directory_button = true;
			await FileManager.CreateDirectoryAsync($"{CurrentPath}{NewDirectoryName}");
			await ChangeDirectoryAsync(CurrentPath);
			NewDirectoryName = string.Empty;
			SelectedDirectories.Clear();
			SelectedFiles.Clear();
		}
		catch (AlertAbleException e)
		{
			await _init_tcs.Task;
			Console.WriteLine(e);
			await _jsop.AlertAsync(e.Message);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
		finally
		{
			_disable_create_directory_button = false;
		}
	}
	#endregion

	/// <summary>
	///		标题的选择框的值
	/// </summary>
	private bool TitleCheckboxValue
	{
		get
		{
			return SelectedFiles.Count == FileList.Count && SelectedDirectories.Count == DirectoryList.Count;
		}
		set
		{
			if (value)
			{
				lock (FileList)
				{
					foreach (IFileInfos file in FileList)
					{
						SelectedFiles.Add(file);
					}
				}

				lock (DirectoryList)
				{
					foreach (IDirectoryInfos dir in DirectoryList)
					{
						SelectedDirectories.Add(dir);
					}
				}
			}
			else
			{
				SelectedDirectories.Clear();
				SelectedFiles.Clear();
			}

			StateHasChanged();
		}
	}

	/// <summary>
	///		用户点击了重命名按钮或按下 F2 就会触发此事件。会传入要被重命名的项的名称。
	/// </summary>
	public static event Action<string>? RenameEvent;

	private async Task OnAnyKeyPressAsync(KeyboardEventArgs e)
	{
		if (e.Code == "F2")
		{
			await OnF2KeyPressAsync();
		}
	}

	private async Task OnF2KeyPressAsync()
	{
		await Task.CompletedTask;
		Console.WriteLine("F2 被按下");
		IDirectoryInfos? dir = SelectedDirectories.FirstOrDefault();
		if (dir is not null)
		{
			RenameEvent?.Invoke(dir.Name);
			return;
		}

		IFileInfos? file = SelectedFiles.FirstOrDefault();
		if (file is not null)
		{
			RenameEvent?.Invoke(file.Name);
			return;
		}
	}
}
