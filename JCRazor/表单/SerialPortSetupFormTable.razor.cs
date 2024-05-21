using JCNET;
using Microsoft.AspNetCore.Components;
using System.IO.Ports;

namespace JCRazor.表单;

#pragma warning disable CA1416 // 验证平台兼容性

public struct SerialPortOptions
{
	public SerialPortOptions()
	{

	}

	public string PortName { get; set; } = string.Empty;
	public int BaudRate { get; set; } = 115200;
	public Parity Parity { get; set; } = Parity.Even;
	public StopBits StopBits { get; set; } = StopBits.One;
}

public partial class SerialPortSetupFormTable
{
	[Parameter]
	public EventCallback<SerialPortOptions> ConnectButtonClickCallback { get; set; }

	private SerialPortOptions _serial_port_options = new();
	private SemaphoreSlim _connect_lock = new(1);

	public string[] PortNames
	{
		get
		{
			string[] names = SerialPort.GetPortNames();
			if (names.Length > 0 && _serial_port_options.PortName == string.Empty)
			{
				_serial_port_options.PortName = names[0];
				StateHasChanged();
			}

			return names;
		}
	}

	public string BaudRateString
	{
		get
		{
			return _serial_port_options.BaudRate.ToString();
		}
		set
		{
			bool parse_result = int.TryParse(value, out int parse_out);
			if (parse_result)
			{
				_serial_port_options.BaudRate = parse_out;
			}
		}
	}

	private async Task OnConnectButtonClickAsync()
	{
		if (_serial_port_options.PortName == string.Empty)
		{
			return;
		}

		try
		{
			using LockGuard l = new(_connect_lock);
			await l.WaitAsync();
			StateHasChanged();
			await ConnectButtonClickCallback.InvokeAsync(_serial_port_options);
		}
		catch
		{

		}
	}
}

#pragma warning restore CA1416 // 验证平台兼容性
