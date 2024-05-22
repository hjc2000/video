using JCNET;
using JCNET.Modbus;
using JCNET.流;
using JCRazor.表单;
using libsdv3.Modbus;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Data;

public interface IDataUpdater
{
	/// <summary>
	///		接口的实现者在这里更新自己需要的数据
	/// </summary>
	/// <returns></returns>
	Task UpdateDatasAsync();
}

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
					foreach (IDataUpdater updater in _data_updater_list)
					{
						await updater.UpdateDatasAsync();
					}

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
	public static IModbusSdv3Device? SDV3 { get; set; }

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

	private static List<IDataUpdater> _data_updater_list = [];
	public static void AddUpdater(IDataUpdater updater)
	{
		lock (_data_updater_list)
		{
			_data_updater_list.Add(updater);
		}
	}
	public static bool RemoveUpdater(IDataUpdater updater)
	{
		lock (_data_updater_list)
		{
			return _data_updater_list.Remove(updater);
		}
	}

	/// <summary>
	///		传入用来更新伺服属性的委托，会按照重试流程和异常处理流程回调传入的委托。
	/// </summary>
	/// <param name="update_func"></param>
	/// <returns></returns>
	public static async Task TryUpdateAsync(Func<Task> update_func)
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
			SDV3 = new SerialPortModbusSdv3Device(new SerialPortStream(SerialPort), 1, true);
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
}
