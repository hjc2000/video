using JCRazor.表单;
using libsdv3.Modbus;
using System;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class TestPage : IAsyncDisposable
{
	#region 生命周期
	private bool _disposed = false;
	public async ValueTask DisposeAsync()
	{
		if (_disposed)
		{
			return;
		}

		_disposed = true;
		GC.SuppressFinalize(this);

		if (_sdv3 is not null)
		{
			await _sdv3.DisposeAsync();
		}

		_cancel_timer.Cancel();
	}
	#endregion

	private ModbusSdv3Device? _sdv3;
	private CancellationTokenSource _cancel_timer = new();

	/// <summary>
	///		连接按钮点击事件
	/// </summary>
	/// <param name="serial_port_options"></param>
	/// <returns></returns>
	private async Task OnConnecteButtonClickAsync(SerialPortOptions serial_port_options)
	{
		try
		{
			if (_sdv3 is not null)
			{
				await _sdv3.DisposeAsync();
			}

			SerialPort serial = new(serial_port_options.PortName)
			{
				BaudRate = serial_port_options.BaudRate,
				Parity = serial_port_options.Parity,
				StopBits = serial_port_options.StopBits,
				ReadTimeout = 2000,
				WriteTimeout = 2000
			};

			await Task.Run(serial.Open);
			_sdv3 = new ModbusSdv3Device(serial.BaseStream, 1, true);

			// 设置定时器
			_cancel_timer.Cancel();
			_cancel_timer = new CancellationTokenSource();
			JCNET.定时器.Timer.SetInterval(async () =>
			{
				try
				{
					Enabled = await _sdv3.GetEI9Async();
					FeedbackCurrentPosition = await _sdv3.GetFeedbackCurrentPositionAsync();
					FeedbackSpeed = await _sdv3.GetFeedbackSpeedAsync();
					P1_01 = await _sdv3.GetPnAsync(1, 1);
					P3_01 = await _sdv3.GetPnAsync(3, 1);
					P3_09 = await _sdv3.GetPnAsync(3, 9);
					P3_10 = await _sdv3.GetPnAsync(3, 10);
					P3_11 = await _sdv3.GetPnAsync(3, 11);
					P3_12 = await _sdv3.GetPnAsync(3, 12);
					Speed = await _sdv3.GetSpeedAsync();
				}
				catch
				{

				}

				await InvokeAsync(StateHasChanged);
			}, 1000, _cancel_timer.Token);
		}
		catch
		{

		}
	}

	private bool Enabled { get; set; } = false;
	private int FeedbackCurrentPosition { get; set; } = 0;
	private int FeedbackSpeed { get; set; } = 0;

	private async Task OnEnableButtonClickAsync()
	{
		try
		{
			if (_sdv3 is null)
			{
				return;
			}

			await _sdv3.SetEI9Async(!await _sdv3.GetEI9Async());
			await _sdv3.SetEI10Async(true);
			await _sdv3.SetEI11Async(false);
		}
		catch
		{

		}
	}

	private async Task OnFwdClickAsync()
	{
		try
		{
			if (_sdv3 is null)
			{
				return;
			}

			await _sdv3.SetEI12Async(!await _sdv3.GetEI12Async());
		}
		catch
		{

		}
	}

	private uint P1_01 { get; set; } = 0;
	private async Task SetP1_01Async(uint value)
	{
		if (_sdv3 is null)
		{
			return;
		}

		try
		{
			await _sdv3.SetPnAsync(1, 1, value);
			P1_01 = value;
		}
		catch
		{

		}
	}

	private uint P3_01 { get; set; } = 0;
	public async Task SetP3_01Async(uint value)
	{
		if (_sdv3 is null)
		{
			return;
		}

		try
		{
			await _sdv3.SetPnAsync(3, 1, value);
			P3_01 = value;
		}
		catch
		{

		}
	}

	private uint P3_09 { get; set; } = 0;
	public async Task SetP3_09Async(uint value)
	{
		if (_sdv3 is null)
		{
			return;
		}

		try
		{
			await _sdv3.SetPnAsync(3, 9, value);
			P3_09 = value;
		}
		catch
		{

		}
	}

	private uint P3_10 { get; set; } = 0;
	public async Task SetP3_10Async(uint value)
	{
		if (_sdv3 is null)
		{
			return;
		}

		try
		{
			await _sdv3.SetPnAsync(3, 10, value);
			P3_10 = value;
		}
		catch
		{

		}
	}

	private uint P3_11 { get; set; } = 0;
	public async Task SetP3_11Async(uint value)
	{
		if (_sdv3 is null)
		{
			return;
		}

		try
		{
			await _sdv3.SetPnAsync(3, 11, value);
			P3_11 = value;
		}
		catch
		{

		}
	}

	private uint P3_12 { get; set; } = 0;
	public async Task SetP3_12Async(uint value)
	{
		if (_sdv3 is null)
		{
			return;
		}

		try
		{
			await _sdv3.SetPnAsync(3, 12, value);
			P3_12 = value;
		}
		catch
		{

		}
	}

	private int Speed { get; set; } = 0;
	private async Task SetSpeedAsync(int value)
	{
		if (_sdv3 is null)
		{
			return;
		}

		try
		{
			await _sdv3.SetSpeedAsync(value);
			Speed = value;
		}
		catch { }
	}
}
