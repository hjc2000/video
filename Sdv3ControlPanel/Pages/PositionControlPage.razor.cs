using JCNET.定时器;
using Sdv3ControlPanel.Data;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class PositionControlPage : IAsyncDisposable
{
	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		if (Database.SDV3 is not null)
		{
			// 设置定时器
			_cancel_timer.Cancel();
			_cancel_timer = new CancellationTokenSource();
			TaskTimer.SetInterval(UpdateDatasAsync, 1000, _cancel_timer.Token);
		}
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

		await _cancel_timer.CancelAsync();
	}

	private static CancellationTokenSource _cancel_timer = new();

	private async Task ConfigAsync()
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		// 定位运行模式
		await SetP1_01Async(7);

		// 内部定位数据无效
		await SetP2_40Async(0);

		// 将 EI9 配置为使能
		await SetP3_09Async(1);

		// 将 EI10 设置为定位数据启动
		await SetP3_10Async(4);
	}

	private async Task UpdateDatasAsync()
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			P1_01 = await Database.SDV3.GetPnAsync(1, 1);
			P2_40 = await Database.SDV3.GetPnAsync(2, 40);
			P3_09 = await Database.SDV3.GetPnAsync(3, 9);
			P3_10 = await Database.SDV3.GetPnAsync(3, 10);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}

		await InvokeAsync(StateHasChanged);
	}

	/// <summary>
	///		运行模式
	/// </summary>
	public static uint P1_01 { get; set; } = 0;
	public static async Task SetP1_01Async(uint value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetPnAsync(1, 1, value);
			P1_01 = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static uint P2_40 { get; set; } = 0;
	public static async Task SetP2_40Async(uint value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetPnAsync(2, 40, value);
			P2_40 = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static uint P3_09 { get; set; } = 0;
	public static async Task SetP3_09Async(uint value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetPnAsync(3, 9, value);
			P3_09 = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static uint P3_10 { get; set; } = 0;
	public static async Task SetP3_10Async(uint value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetPnAsync(3, 10, value);
			P3_10 = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}
}
