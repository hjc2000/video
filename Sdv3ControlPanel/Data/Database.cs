using JCNET;
using JCNET.Modbus;
using JCNET.流;
using JCRazor.表单;
using libsdv3.Modbus;
using System;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Data;

public static class Database
{
	/// <summary>
	///		连接到设备
	/// </summary>
	/// <param name="serial_port_options"></param>
	/// <returns></returns>
	public static async Task ConnectAsync(SerialPortOptions serial_port_options)
	{
		try
		{
			SerialPortOptions = serial_port_options;
			if (SDV3 is not null)
			{
				await SDV3.DisposeAsync();
			}

			SerialPort?.Dispose();

			// 设置定时器
			_cancel_timer.Cancel();
			_cancel_timer = new CancellationTokenSource();
			_ = Task.Run(async () =>
			{
				while (!_cancel_timer.IsCancellationRequested)
				{
					await UpdateDatas();
					await Task.Delay(1000);
				}
			}, _cancel_timer.Token);
		}
		catch (Exception ex)
		{
			LogOutputPort.WriteLine(ex);
		}
	}

	private static CancellationTokenSource _cancel_timer = new();
	private static SerialPort? SerialPort { get; set; }
	private static SerialPortOptions SerialPortOptions { get; set; } = new();
	public static ModbusSdv3Device? SDV3 { get; set; }

	private static LogOutputPort? _log_output_port = null;
	private static readonly object _log_lock = new();
	public static LogOutputPort LogOutputPort
	{
		get
		{
			if (_log_output_port is null)
			{
				lock (_log_lock)
				{
					_log_output_port ??= new LogOutputPort("log.txt");
				}
			}

			return _log_output_port;
		}
	}

	public static event Action? DataUpdatedEvent;

	/// <summary>
	///		传入用来更新伺服属性的委托，会按照重试流程和异常处理流程回调传入的委托。
	/// </summary>
	/// <param name="update_func"></param>
	/// <returns></returns>
	private static async Task TryUpdate(Func<Task> update_func)
	{
		static async Task OpenDeviceAsync()
		{
			SerialPort = new(SerialPortOptions.PortName)
			{
				BaudRate = SerialPortOptions.BaudRate,
				Parity = SerialPortOptions.Parity,
				StopBits = SerialPortOptions.StopBits,
				ReadTimeout = 2000,
				WriteTimeout = 2000,
			};

			await Task.Run(SerialPort.Open);

			/* 必须使用 SerialPortStream，因为 SerialPort 的 BaseStream 永远不会触发
			 * 超时。SerialPortStream 内部通过 SerialPort 的同步读写方法来实现流，所以
			 * 可以触发超时。
			 */
			SDV3 = new ModbusSdv3Device(new SerialPortStream(SerialPort), 1, true);
			LogOutputPort.WriteLine("成功打开新的 SDV3 对象");
		}

		uint retry_times = 0;
		while (true)
		{
			try
			{
				if (SerialPort is not null)
				{
					if (!SerialPort.IsOpen)
					{
						if (SDV3 is not null)
						{
							await SDV3.DisposeAsync();
						}

						SerialPort.Dispose();
						SDV3 = null;
						await Task.Delay(1000);
					}
				}

				if (SDV3 is null)
				{
					await OpenDeviceAsync();
				}

				await update_func();
				break;
			}
			catch (ModbusFrameException e)
			{
				LogOutputPort.WriteLine(e);
				try
				{
					if (retry_times >= 3)
					{
						break;
					}

					LogOutputPort.WriteLine("发生 ModbusFrameException，清理接收缓冲区和发送缓冲区中的垃圾数据");
					SerialPort?.DiscardInBuffer();
					SerialPort?.DiscardOutBuffer();
					retry_times++;
					continue;
				}
				catch { }
			}
			catch (Exception e)
			{
				LogOutputPort.WriteLine("发生异常，重新打开串口");
				LogOutputPort.WriteLine(e.ToString());
				if (SDV3 is not null)
				{
					await SDV3.DisposeAsync();
				}

				SerialPort?.Dispose();
				SDV3 = null;
				await Task.Delay(100);
				continue;
			}
		}
	}

	private static async Task UpdateDatas()
	{
		await TryUpdate(async () =>
		{
			Enabled = await SDV3!.GetEI9Async();
		});

		await TryUpdate(async () =>
		{
			FeedbackCurrentPosition = await SDV3!.GetFeedbackCurrentPositionAsync();
		});

		await TryUpdate(async () =>
		{
			FeedbackSpeed = await SDV3!.GetFeedbackSpeedAsync();
		});

		await TryUpdate(async () =>
		{
			P1_01 = await SDV3!.GetPnAsync(1, 1);
		});

		await TryUpdate(async () =>
		{
			P3_01 = await SDV3!.GetPnAsync(3, 1);
		});

		await TryUpdate(async () =>
		{
			P3_09 = await SDV3!.GetPnAsync(3, 9);
		});

		await TryUpdate(async () =>
		{
			P3_10 = await SDV3!.GetPnAsync(3, 10);
		});

		await TryUpdate(async () =>
		{
			P3_11 = await SDV3!.GetPnAsync(3, 11);
		});

		await TryUpdate(async () =>
		{
			P3_12 = await SDV3!.GetPnAsync(3, 12);
		});

		await TryUpdate(async () =>
		{
			Speed = await SDV3!.GetSpeedAsync();
		});

		DataUpdatedEvent?.Invoke();
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
		if (SDV3 is null)
		{
			return;
		}

		try
		{
			await SDV3.SetPnAsync(1, 1, value);
			P1_01 = value;
		}
		catch (Exception ex)
		{
			LogOutputPort.WriteLine(ex);
		}
	}

	public static uint P2_40 { get; set; } = 0;
	public static async Task SetP2_40Async(uint value)
	{
		if (SDV3 is null)
		{
			return;
		}

		try
		{
			await SDV3.SetPnAsync(2, 40, value);
			P2_40 = value;
		}
		catch (Exception ex)
		{
			LogOutputPort.WriteLine(ex);
		}
	}

	public static uint P3_01 { get; set; } = 0;
	public static async Task SetP3_01Async(uint value)
	{
		if (SDV3 is null)
		{
			return;
		}

		try
		{
			await SDV3.SetPnAsync(3, 1, value);
			P3_01 = value;
		}
		catch (Exception ex)
		{
			LogOutputPort.WriteLine(ex);
		}
	}

	public static uint P3_09 { get; set; } = 0;
	public static async Task SetP3_09Async(uint value)
	{
		if (SDV3 is null)
		{
			return;
		}

		try
		{
			await SDV3.SetPnAsync(3, 9, value);
			P3_09 = value;
		}
		catch (Exception ex)
		{
			LogOutputPort.WriteLine(ex);
		}
	}

	public static uint P3_10 { get; set; } = 0;
	public static async Task SetP3_10Async(uint value)
	{
		if (SDV3 is null)
		{
			return;
		}

		try
		{
			await SDV3.SetPnAsync(3, 10, value);
			P3_10 = value;
		}
		catch (Exception ex)
		{
			LogOutputPort.WriteLine(ex);
		}
	}

	public static uint P3_11 { get; set; } = 0;
	public static async Task SetP3_11Async(uint value)
	{
		if (SDV3 is null)
		{
			return;
		}

		try
		{
			await SDV3.SetPnAsync(3, 11, value);
			P3_11 = value;
		}
		catch (Exception ex)
		{
			LogOutputPort.WriteLine(ex);
		}
	}

	public static uint P3_12 { get; set; } = 0;
	public static async Task SetP3_12Async(uint value)
	{
		if (SDV3 is null)
		{
			return;
		}

		try
		{
			await SDV3.SetPnAsync(3, 12, value);
			P3_12 = value;
		}
		catch (Exception ex)
		{
			LogOutputPort.WriteLine(ex);
		}
	}

	public static int Speed { get; set; } = 0;
	public static async Task SetSpeedAsync(int value)
	{
		if (SDV3 is null)
		{
			return;
		}

		try
		{
			await SDV3.SetSpeedAsync(value);
			Speed = value;
		}
		catch (Exception ex)
		{
			LogOutputPort.WriteLine(ex);
		}
	}
}
