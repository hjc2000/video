using JCNET;
using JCRazor.表单;
using libsdv3.Modbus;
using System;
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
