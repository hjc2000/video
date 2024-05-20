using Microsoft.AspNetCore.Components;
using System.IO.Ports;

namespace JCRazor.表单;

#pragma warning disable CA1416 // 验证平台兼容性

public partial class SerialPortSetupFormTable
{
	/// <summary>
	///		用户点击连接按钮后会触发此事件，本组件会构造一个 SerialPort
	///		对象并传递给你。这个 SerialPort 还没有调用 Open 方法打开。
	/// </summary>
	[Parameter]
	public EventCallback<SerialPort> ConnectButtonClickCallback { get; set; }

	public string[] PortNames
	{
		get
		{
			string[] names = SerialPort.GetPortNames();
			if (names.Length > 0 && SelectedPortName == string.Empty)
			{
				SelectedPortName = names[0];
				StateHasChanged();
			}

			return names;
		}
	}

	private string SelectedPortName { get; set; } = string.Empty;

	private int BaudRate { get; set; } = 115200;
	public string BaudRateString
	{
		get
		{
			return BaudRate.ToString();
		}
		set
		{
			bool parse_result = int.TryParse(value, out int parse_out);
			if (parse_result)
			{
				BaudRate = parse_out;
			}
		}
	}

	private Parity ParityValue { get; set; } = Parity.Even;
	private StopBits StopBitsValue { get; set; } = StopBits.One;

	private async Task OnConnectButtonClickAsync()
	{
		if (SelectedPortName == string.Empty)
		{
			return;
		}

		SerialPort serial = new(SelectedPortName)
		{
			BaudRate = BaudRate,
			Parity = ParityValue,
			StopBits = StopBitsValue,
		};
		await ConnectButtonClickCallback.InvokeAsync(serial);
	}
}

#pragma warning restore CA1416 // 验证平台兼容性
