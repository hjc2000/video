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
			TaskTimer.SetInterval(UpdateDatasAsync, 100, _cancel_timer.Token);
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

		// 将 EI11 设置为位置预置功能
		await SetP3_11Async(16);

		// 将 EI12 设置为立即值变更指令
		await SetP3_12Async(23);

		// 将 EI13 设置为临时停止
		await SetP3_13Async(31);

		// 将 EI14 设置为定位取消
		await SetP3_14Async(32);
	}

	private bool CancelPositioning { get; set; } = false;
	private async Task SetCancelPositioningAsync(bool value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetEI14Async(value);
			CancelPositioning = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	private async Task SetImmediateDatasAsync()
	{
		// 使用绝对定位模式
		await SetImmediateStateAsync(0);
		await SetImmediatePositionAsync(100000);
		await SetImmediateSpeedAsync(10000);
		await SetImmediateAccelerationDurationAsync(100);
		await SetImmediateDecelerationDurationAsync(100);
	}

	private async Task PresetPositionAsync()
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		await Database.SDV3.SetEI11Async(false);
		await Database.SDV3.SetEI11Async(true);
	}

	private async Task StartPositioningAsync()
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetEI10Async(false);
			await Database.SDV3.SetEI10Async(true);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	private async Task UpdateDatasAsync()
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			FeedbackCurrentPosition = await Database.SDV3.GetFeedbackCurrentPositionAsync();
			FeedbackSpeed = await Database.SDV3.GetFeedbackSpeedAsync();
			P1_01 = await Database.SDV3.GetPnAsync(1, 1);
			P2_40 = await Database.SDV3.GetPnAsync(2, 40);
			P3_09 = await Database.SDV3.GetPnAsync(3, 9);
			P3_10 = await Database.SDV3.GetPnAsync(3, 10);
			P3_11 = await Database.SDV3.GetPnAsync(3, 11);
			P3_12 = await Database.SDV3.GetPnAsync(3, 12);
			P3_13 = await Database.SDV3.GetPnAsync(3, 13);
			P3_14 = await Database.SDV3.GetPnAsync(3, 14);
			ImmediatePosition = await Database.SDV3.GetImmediatePositionAsync();
			ImmediateSpeed = await Database.SDV3.GetImmediateSpeedAsync();
			ImmediateAccelerationDuration = await Database.SDV3.GetImmediateAccelerationDurationAsync();
			ImmediateDecelerationDuration = await Database.SDV3.GetImmediateDecelerationDurationAsync();
			CancelPositioning = await Database.SDV3.GetEI14Async();
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}

		await InvokeAsync(StateHasChanged);
	}

	public static async Task ForceUpdateImmediateDatasAsync()
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetEI12Async(false);
			await Database.SDV3.SetEI12Async(true);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static int FeedbackCurrentPosition { get; set; } = 0;
	public static int FeedbackSpeed { get; set; } = 0;

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

	public static uint P3_11 { get; set; } = 0;
	public static async Task SetP3_11Async(uint value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetPnAsync(3, 11, value);
			P3_11 = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static uint P3_12 { get; set; } = 0;
	public static async Task SetP3_12Async(uint value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetPnAsync(3, 12, value);
			P3_12 = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static uint P3_13 { get; set; } = 0;
	public static async Task SetP3_13Async(uint value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetPnAsync(3, 13, value);
			P3_13 = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static uint P3_14 { get; set; } = 0;
	public static async Task SetP3_14Async(uint value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetPnAsync(3, 14, value);
			P3_14 = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static async Task SetImmediateStateAsync(uint value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetImmediateStateAsync(value);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static int ImmediatePosition { get; set; } = 0;
	public static async Task SetImmediatePositionAsync(int value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetImmediatePositionAsync(value);
			ImmediatePosition = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static int ImmediateSpeed { get; set; } = 0;
	public static async Task SetImmediateSpeedAsync(int value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetImmediateSpeedAsync(value);
			ImmediateSpeed = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static int ImmediateAccelerationDuration { get; set; } = 0;
	public static async Task SetImmediateAccelerationDurationAsync(int value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetImmediateAccelerationDurationAsync(value);
			ImmediateAccelerationDuration = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

	public static int ImmediateDecelerationDuration { get; set; } = 0;
	public static async Task SetImmediateDecelerationDurationAsync(int value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetImmediateDecelerationDurationAsync(value);
			ImmediateDecelerationDuration = value;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}
}
