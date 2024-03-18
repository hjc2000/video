using JCNET;
using JCNET.容器;
using JCRazor.WindowComponents;
using Microsoft.AspNetCore.Components;
using Microsoft.JSInterop;

namespace JCRazor.文件浏览器;
public partial class ItemOperator : IAsyncDisposable
{
	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		_jsop = new JSOp(JSRuntime);
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
		if (_jsop is not null)
		{
			await _jsop.DisposeAsync();
		}
	}

	[Inject]
	private IJSRuntime JSRuntime { get; set; } = default!;
	private TaskCompletionSource _init_tcs = new();
	private JSOp _jsop = default!;
	private SemaphoreSlim _opetator_lock = new(1, 1);

	[Parameter]
	public IFileManager FileManager { get; set; } = default!;

	public ConcurrentHashSet<IFileInfos> SelectedFiles { get; } = [];
	public ConcurrentHashSet<IDirectoryInfos> SelectedDirectories { get; } = [];

	/// <summary>
	///		将文件、文件夹移动到指定的基路径下。
	/// </summary>
	/// <param name="dst_base_path">基路径</param>
	/// <returns></returns>
	public async Task MoveToAsync(string dst_base_path)
	{
		try
		{
			using LockGuard l = new(_opetator_lock);
			await l.WaitAsync();
			if (!dst_base_path.EndsWith('/'))
			{
				dst_base_path += "/";
			}

			if (SelectedDirectories.Count + SelectedFiles.Count == 0)
			{
				return;
			}

			await _operating_list_dialog.ShowModalAsync();
			while (true)
			{
				IDirectoryInfos? first = SelectedDirectories.FirstOrDefault();
				if (first is null)
				{
					break;
				}

				string new_full_path = $"{dst_base_path}{first.Name}/";
				await FileManager.MoveAsync(first.FullPath, new_full_path, false);
				SelectedDirectories.Remove(first);
				StateHasChanged();
			}

			while (true)
			{
				IFileInfos? first = SelectedFiles.FirstOrDefault();
				if (first is null)
				{
					break;
				}

				string new_full_path = $"{dst_base_path}{first.Name}";
				await FileManager.MoveAsync(first.FullPath, new_full_path, false);
				SelectedFiles.Remove(first);
				StateHasChanged();
			}
		}
		catch (AlertAbleException e)
		{
			await _init_tcs.Task;
			await _jsop.AlertAsync(e.Message);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
		finally
		{
			await _operating_list_dialog.CloseAsync();
		}
	}

	public async Task CopyToAsync(string dst_base_path)
	{
		try
		{
			using LockGuard l = new(_opetator_lock);
			await l.WaitAsync();
			if (!dst_base_path.EndsWith('/'))
			{
				dst_base_path += "/";
			}

			if (SelectedDirectories.Count + SelectedFiles.Count == 0)
			{
				return;
			}

			await _operating_list_dialog.ShowModalAsync();
			while (true)
			{
				IDirectoryInfos? first = SelectedDirectories.FirstOrDefault();
				if (first is null)
				{
					break;
				}

				string new_full_path = $"{dst_base_path}{first.Name}/";
				await FileManager.CopyAsync(first.FullPath, new_full_path, false);
				SelectedDirectories.Remove(first);
				StateHasChanged();
			}

			while (true)
			{
				IFileInfos? first = SelectedFiles.FirstOrDefault();
				if (first is null)
				{
					break;
				}

				string new_full_path = $"{dst_base_path}{first.Name}";
				await FileManager.CopyAsync(first.FullPath, new_full_path, false);
				SelectedFiles.Remove(first);
				StateHasChanged();
			}
		}
		catch (AlertAbleException e)
		{
			await _init_tcs.Task;
			await _jsop.AlertAsync(e.Message);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
		finally
		{
			await _operating_list_dialog.CloseAsync();
		}
	}

	#region 删除文件
	private EnsureDialog _delete_ensure_dialog = default!;
	private Dialog _operating_list_dialog = default!;
	private TaskCompletionSource<bool> _ensure_delete_tcs = new();

	private async Task OnEnsureCallbackAsync(bool yes_or_no)
	{
		await Task.CompletedTask;
		_ensure_delete_tcs.TrySetResult(yes_or_no);
	}

	public async Task DeleteAsync()
	{
		try
		{
			using LockGuard l = new(_opetator_lock);
			await l.WaitAsync();
			if (SelectedDirectories.Count + SelectedFiles.Count == 0)
			{
				return;
			}

			// 询问是否确认删除
			_ensure_delete_tcs = new TaskCompletionSource<bool>();
			await _delete_ensure_dialog.ShowModalAsync();
			bool want_to_delete = await _ensure_delete_tcs.Task;
			await _delete_ensure_dialog.CloseAsync();
			if (!want_to_delete)
			{
				Console.WriteLine("取消删除");
				return;
			}

			// 确认完毕，确定要删除
			await _operating_list_dialog.ShowModalAsync();
			while (true)
			{
				IDirectoryInfos? first = SelectedDirectories.FirstOrDefault();
				if (first is null)
				{
					break;
				}

				await FileManager.DeleteAsync(first.FullPath);
				SelectedDirectories.Remove(first);
				StateHasChanged();
			}

			while (true)
			{
				IFileInfos? first = SelectedFiles.FirstOrDefault();
				if (first is null)
				{
					break;
				}

				await FileManager.DeleteAsync(first.FullPath);
				SelectedFiles.Remove(first);
				StateHasChanged();
			}
		}
		catch (AlertAbleException e)
		{
			await _init_tcs.Task;
			await _jsop.AlertAsync(e.Message);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
		finally
		{
			await _operating_list_dialog.CloseAsync();
		}
	}
	#endregion
}
