using JCNET;
using JCRazor.表单;
using libsdv3.Modbus;
using System;
using System.IO;
using System.IO.Ports;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Data;

public class Database
{
	public static SerialPort? SerialPort { get; set; }
	public static ModbusSdv3Device? SDV3 { get; set; }
	public static SerialPortOptions SerialPortOptions { get; set; } = new();

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

	/// <summary>
	///		传入用来更新伺服属性的委托，会按照重试流程和异常处理流程回调传入的委托。
	/// </summary>
	/// <param name="update_func"></param>
	/// <returns></returns>
	public static async Task TryUpdate(Func<Task> update_func)
	{
		uint retry_times = 0;
		while (true)
		{
			try
			{
				if (SDV3 is null)
				{
					SerialPort = new(SerialPortOptions.PortName)
					{
						BaudRate = SerialPortOptions.BaudRate,
						Parity = SerialPortOptions.Parity,
						StopBits = SerialPortOptions.StopBits,
					};

					await Task.Run(SerialPort.Open);
					SDV3 = new ModbusSdv3Device(SerialPort.BaseStream, 1, true);
					LogOutputPort.WriteLine("成功打开新的 SDV3 对象");
				}

				await update_func();
				break;
			}
			catch (IOException e)
			{
				LogOutputPort.WriteLine(e);
				try
				{
					if (retry_times >= 3)
					{
						break;
					}

					LogOutputPort.WriteLine("发生 IOException，清理接收缓冲区和发送缓冲区中的垃圾数据");
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

				SDV3 = null;
				await Task.Delay(100);
				continue;
			}
		}
	}

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
