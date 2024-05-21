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
	public int BaudRate { get; set; } = 0;
	public Parity Parity { get; set; } = Parity.None;
	public StopBits StopBits { get; set; } = StopBits.One;
}

public partial class SerialPortSetupFormTable
{
	/// <summary>
	///		用户点击连接按钮后会触发此事件，本组件会构造一个 SerialPort
	///		对象并传递给你。这个 SerialPort 还没有调用 Open 方法打开。
	/// </summary>
	[Parameter]
	public EventCallback<SerialPort> ConnectButtonClickCallback { get; set; }

	private SerialPortOptions _serial_port_options;

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

	private SemaphoreSlim _connect_lock = new(1);
	private async Task OnConnectButtonClickAsync()
	{
		if (_serial_port_options.PortName == string.Empty)
		{
			return;
		}

		using LockGuard l = new(_connect_lock);
		await l.WaitAsync();
		StateHasChanged();
		try
		{
			SerialPort serial = new(_serial_port_options.PortName)
			{
				BaudRate = _serial_port_options.BaudRate,
				Parity = _serial_port_options.Parity,
				StopBits = _serial_port_options.StopBits,
			};

			await ConnectButtonClickCallback.InvokeAsync(serial);
		}
		catch
		{

		}
	}
}

#pragma warning restore CA1416 // 验证平台兼容性
