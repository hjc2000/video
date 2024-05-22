using JCNET;
using JCRazor.表单;
using libsdv3.Modbus;
using System.IO.Ports;

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
}
