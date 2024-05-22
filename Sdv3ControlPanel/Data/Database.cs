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
			if (SDV3 is not null)
			{
				await SDV3.DisposeAsync();
			}

			SerialPort serial_port = new(serial_port_options.PortName)
			{
				BaudRate = serial_port_options.BaudRate,
				Parity = serial_port_options.Parity,
				StopBits = serial_port_options.StopBits,
				ReadTimeout = 2000,
				WriteTimeout = 2000,
			};

			SDV3 = new SerialPortModbusSdv3Device(serial_port, 1, true);
			Console.WriteLine("成功打开新的 SDV3 对象");

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
			Console.WriteLine(ex);
		}
	}

	private static CancellationTokenSource _cancel_timer = new();
	public static IModbusSdv3Device? SDV3 { get; set; }

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
}
