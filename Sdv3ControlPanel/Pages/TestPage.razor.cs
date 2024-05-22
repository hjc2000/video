using JCNET;
using Microsoft.AspNetCore.Components;
using Sdv3ControlPanel.Data;
using System;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class TestPage : IAsyncDisposable, IDataUpdater
{
	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		Database.AddUpdater(this);
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
		Database.RemoveUpdater(this);
	}

	[Inject]
	public LogOutputPort LogOutputPort { get; set; } = default!;

	/// <summary>
	///		配置为速度控制模式并能指定速度
	/// </summary>
	/// <returns></returns>
	private async Task ConfigAsync()
	{
		await SetP1_01Async(1);
		await SetP3_01Async(0);
		await SetP3_09Async(1);
		await SetP3_10Async(18);
		await SetP3_11Async(2);
	}

	/// <summary>
	///		使能按钮点击事件处理函数
	/// </summary>
	/// <returns></returns>
	private async Task OnEnableButtonClickAsync()
	{
		try
		{
			if (Database.SDV3 is null)
			{
				return;
			}

			Database.LogOutputPort.WriteLine("使能按钮处理函数");
			await Database.SDV3.SetEI9Async(!await Database.SDV3.GetEI9Async());
			await Database.SDV3.SetEI10Async(true);
		}
		catch (Exception ex)
		{
			Database.LogOutputPort.WriteLine(ex.ToString());
		}
	}

	/// <summary>
	///		正转按钮点击事件处理函数
	/// </summary>
	/// <returns></returns>
	private async Task OnFwdClickAsync()
	{
		try
		{
			if (Database.SDV3 is null)
			{
				return;
			}

			Database.LogOutputPort.WriteLine("正转按钮处理函数");
			await Database.SDV3.SetEI11Async(!await Database.SDV3.GetEI11Async());
		}
		catch (Exception ex)
		{
			Database.LogOutputPort.WriteLine(ex.ToString());
		}
	}

	async Task IDataUpdater.UpdateDatasAsync()
	{
		await Database.TryUpdateAsync(async () =>
		{
			Enabled = await Database.SDV3!.GetEI9Async();
		});

		await Database.TryUpdateAsync(async () =>
		{
			FeedbackCurrentPosition = await Database.SDV3!.GetFeedbackCurrentPositionAsync();
		});

		await Database.TryUpdateAsync(async () =>
		{
			FeedbackSpeed = await Database.SDV3!.GetFeedbackSpeedAsync();
		});

		await Database.TryUpdateAsync(async () =>
		{
			P1_01 = await Database.SDV3!.GetPnAsync(1, 1);
		});

		await Database.TryUpdateAsync(async () =>
		{
			P3_01 = await Database.SDV3!.GetPnAsync(3, 1);
		});

		await Database.TryUpdateAsync(async () =>
		{
			P3_09 = await Database.SDV3!.GetPnAsync(3, 9);
		});

		await Database.TryUpdateAsync(async () =>
		{
			P3_10 = await Database.SDV3!.GetPnAsync(3, 10);
		});

		await Database.TryUpdateAsync(async () =>
		{
			P3_11 = await Database.SDV3!.GetPnAsync(3, 11);
		});

		await Database.TryUpdateAsync(async () =>
		{
			P3_12 = await Database.SDV3!.GetPnAsync(3, 12);
		});

		await Database.TryUpdateAsync(async () =>
		{
			Speed = await Database.SDV3!.GetSpeedAsync();
		});

		await InvokeAsync(StateHasChanged);
	}

	public static bool Enabled { get; set; } = false;
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
			Database.LogOutputPort.WriteLine(ex);
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
			Database.LogOutputPort.WriteLine(ex);
		}
	}

	public static uint P3_01 { get; set; } = 0;
	public static async Task SetP3_01Async(uint value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetPnAsync(3, 1, value);
			P3_01 = value;
		}
		catch (Exception ex)
		{
			Database.LogOutputPort.WriteLine(ex);
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
			Database.LogOutputPort.WriteLine(ex);
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
			Database.LogOutputPort.WriteLine(ex);
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
			Database.LogOutputPort.WriteLine(ex);
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
			Database.LogOutputPort.WriteLine(ex);
		}
	}

	public static int Speed { get; set; } = 0;
	public static async Task SetSpeedAsync(int value)
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			await Database.SDV3.SetSpeedAsync(value);
			Speed = value;
		}
		catch (Exception ex)
		{
			Database.LogOutputPort.WriteLine(ex);
		}
	}
}
