using JCNET;
using JCRazor.表单;
using libsdv3.Modbus;
using Sdv3ControlPanel.Data;
using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class TestPage : IAsyncDisposable
{
	#region 生命周期
	public TestPage()
	{
		_log_output_port = new LogOutputPort("log.txt");
		_log_output_port.WriteLine("LogOutputPort 启动。");
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

		if (Database.SDV3 is not null)
		{
			await Database.SDV3.DisposeAsync();
		}

		_cancel_timer.Cancel();
		await _log_output_port.DisposeAsync();
	}
	#endregion

	private CancellationTokenSource _cancel_timer = new();
	private SerialPortOptions _serial_port_options = new();
	private LogOutputPort _log_output_port;

	/// <summary>
	///		连接按钮点击事件
	/// </summary>
	/// <param name="serial_port_options"></param>
	/// <returns></returns>
	private async Task OnConnecteButtonClickAsync(SerialPortOptions serial_port_options)
	{
		try
		{
			_serial_port_options = serial_port_options;
			if (Database.SDV3 is not null)
			{
				await Database.SDV3.DisposeAsync();
				_log_output_port.WriteLine("释放旧的 Database.SDV3 对象");
			}

			// 设置定时器
			_cancel_timer.Cancel();
			_cancel_timer = new CancellationTokenSource();
			_ = Task.Run(async () =>
			{
				while (!_cancel_timer.IsCancellationRequested)
				{
					await TimerElapsedEventHandler();
					await Task.Delay(1000);
				}
			}, _cancel_timer.Token);
		}
		catch (Exception ex)
		{
			_log_output_port.WriteLine(ex);
		}
	}

	private bool Enabled { get; set; } = false;
	private int FeedbackCurrentPosition { get; set; } = 0;
	private int FeedbackSpeed { get; set; } = 0;

	private async Task TryUpdate(Func<Task> update_func)
	{
		uint retry_times = 0;
		while (true)
		{
			try
			{
				if (Database.SDV3 is null)
				{
					Database.SerialPort = new(_serial_port_options.PortName)
					{
						BaudRate = _serial_port_options.BaudRate,
						Parity = _serial_port_options.Parity,
						StopBits = _serial_port_options.StopBits,
					};

					await Task.Run(Database.SerialPort.Open);
					Database.SDV3 = new ModbusSdv3Device(Database.SerialPort.BaseStream, 1, true);
					_log_output_port.WriteLine("成功打开新的 Database.SDV3 对象");
				}

				await update_func();
				break;
			}
			catch (IOException e)
			{
				_log_output_port.WriteLine(e);
				try
				{
					if (retry_times >= 3)
					{
						break;
					}

					_log_output_port.WriteLine("发生 IOException，清理接收缓冲区和发送缓冲区中的垃圾数据");
					Database.SerialPort?.DiscardInBuffer();
					Database.SerialPort?.DiscardOutBuffer();
					retry_times++;
					continue;
				}
				catch { }
			}
			catch (Exception e)
			{
				_log_output_port.WriteLine("发生异常，重新打开串口");
				_log_output_port.WriteLine(e.ToString());
				if (Database.SDV3 is not null)
				{
					await Database.SDV3.DisposeAsync();
				}

				Database.SDV3 = null;
				await Task.Delay(100);
				continue;
			}
		}
	}

	/// <summary>
	///		定时任务
	/// </summary>
	/// <returns></returns>
	private async Task TimerElapsedEventHandler()
	{
		await TryUpdate(async () =>
		{
			Enabled = await Database.SDV3!.GetEI9Async();
		});

		await TryUpdate(async () =>
		{
			FeedbackCurrentPosition = await Database.SDV3!.GetFeedbackCurrentPositionAsync();
		});

		await TryUpdate(async () =>
		{
			FeedbackSpeed = await Database.SDV3!.GetFeedbackSpeedAsync();
		});

		await TryUpdate(async () =>
		{
			P1_01 = await Database.SDV3!.GetPnAsync(1, 1);
		});

		await TryUpdate(async () =>
		{
			P3_01 = await Database.SDV3!.GetPnAsync(3, 1);
		});

		await TryUpdate(async () =>
		{
			P3_09 = await Database.SDV3!.GetPnAsync(3, 9);
		});

		await TryUpdate(async () =>
		{
			P3_10 = await Database.SDV3!.GetPnAsync(3, 10);
		});

		await TryUpdate(async () =>
		{
			P3_11 = await Database.SDV3!.GetPnAsync(3, 11);
		});

		await TryUpdate(async () =>
		{
			P3_12 = await Database.SDV3!.GetPnAsync(3, 12);
		});

		await TryUpdate(async () =>
		{
			Speed = await Database.SDV3!.GetSpeedAsync();
		});

		await InvokeAsync(StateHasChanged);
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

			_log_output_port.WriteLine("使能按钮处理函数");
			await Database.SDV3.SetEI9Async(!await Database.SDV3.GetEI9Async());
			await Database.SDV3.SetEI10Async(true);
			await Database.SDV3.SetEI11Async(false);
		}
		catch (Exception ex)
		{
			_log_output_port.WriteLine(ex.ToString());
		}
	}

	private async Task OnFwdClickAsync()
	{
		try
		{
			if (Database.SDV3 is null)
			{
				return;
			}

			_log_output_port.WriteLine("正转按钮处理函数");
			await Database.SDV3.SetEI12Async(!await Database.SDV3.GetEI12Async());
		}
		catch (Exception ex)
		{
			_log_output_port.WriteLine(ex.ToString());
		}
	}

	private uint P1_01 { get; set; } = 0;
	private async Task SetP1_01Async(uint value)
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
			_log_output_port.WriteLine(ex);
		}
	}

	private uint P3_01 { get; set; } = 0;
	public async Task SetP3_01Async(uint value)
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
			_log_output_port.WriteLine(ex);
		}
	}

	private uint P3_09 { get; set; } = 0;
	public async Task SetP3_09Async(uint value)
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
			_log_output_port.WriteLine(ex);
		}
	}

	private uint P3_10 { get; set; } = 0;
	public async Task SetP3_10Async(uint value)
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
			_log_output_port.WriteLine(ex);
		}
	}

	private uint P3_11 { get; set; } = 0;
	public async Task SetP3_11Async(uint value)
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
			_log_output_port.WriteLine(ex);
		}
	}

	private uint P3_12 { get; set; } = 0;
	public async Task SetP3_12Async(uint value)
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
			_log_output_port.WriteLine(ex);
		}
	}

	private int Speed { get; set; } = 0;
	private async Task SetSpeedAsync(int value)
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
			_log_output_port.WriteLine(ex);
		}
	}
}
