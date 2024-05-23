using JCNET.定时器;
using JCRazor.表单;
using libsdv3.Modbus;
using Sdv3ControlPanel.Data;
using System;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class ConfigConnectionPage : IAsyncDisposable
{
	#region 生命周期
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
	#endregion

	private static CancellationTokenSource _cancel_timer = new();

	/// <summary>
	///		连接到设备
	/// </summary>
	/// <param name="serial_port_options"></param>
	/// <returns></returns>
	public async Task ConnectAsync(SerialPortOptions serial_port_options)
	{
		try
		{
			if (Database.SDV3 is not null)
			{
				await Database.SDV3.DisposeAsync();
			}

			SerialPort serial_port = new(serial_port_options.PortName)
			{
				BaudRate = serial_port_options.BaudRate,
				Parity = serial_port_options.Parity,
				StopBits = serial_port_options.StopBits,
				ReadTimeout = 2000,
				WriteTimeout = 2000,
			};

			Database.SDV3 = new SerialPortModbusSdv3Device(serial_port, 1, true);
			Console.WriteLine("成功打开新的 SDV3 对象");

			// 设置定时器
			_cancel_timer.Cancel();
			_cancel_timer = new CancellationTokenSource();
			TaskTimer.SetInterval(UpdateDatasAsync, 1000, _cancel_timer.Token);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}
	}

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

			Console.WriteLine("使能按钮处理函数");
			await Database.SDV3.SetEI9Async(!await Database.SDV3.GetEI9Async());
			await Database.SDV3.SetEI10Async(true);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
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

			Console.WriteLine("正转按钮处理函数");
			await Database.SDV3.SetEI11Async(!await Database.SDV3.GetEI11Async());
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	/// <summary>
	///		被定时器回调，定时更新伺服的状态
	/// </summary>
	/// <returns></returns>
	private async Task UpdateDatasAsync()
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		try
		{
			Enabled = await Database.SDV3.GetEI9Async();
			FeedbackCurrentPosition = await Database.SDV3.GetFeedbackCurrentPositionAsync();
			FeedbackSpeed = await Database.SDV3.GetFeedbackSpeedAsync();
			P1_01 = await Database.SDV3.GetPnAsync(1, 1);
			P3_01 = await Database.SDV3.GetPnAsync(3, 1);
			P3_09 = await Database.SDV3.GetPnAsync(3, 9);
			P3_10 = await Database.SDV3.GetPnAsync(3, 10);
			P3_11 = await Database.SDV3.GetPnAsync(3, 11);
			P3_12 = await Database.SDV3.GetPnAsync(3, 12);
			Speed = await Database.SDV3.GetSpeedAsync();
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex);
		}

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
			Console.WriteLine(ex);
		}
	}
}
