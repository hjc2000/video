using JCNET.流;
using libsdv3.Modbus;
using System;
using System.IO.Ports;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class TestPage : IAsyncDisposable
{
	public TestPage()
	{
		_serial.Open();
		_sdv3 = new(new SerialPortStream(_serial), 1, true);
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

		await ValueTask.CompletedTask;
		_serial.Close();
		_serial.Dispose();
	}

	private ModbusSdv3Device _sdv3;
	private SerialPort _serial = new("COM5")
	{
		BaudRate = 115200,
		Parity = Parity.Even,
		StopBits = StopBits.One
	};

	private bool Enabled
	{
		get
		{
			try
			{
				return _sdv3.EI9;
			}
			catch
			{
				return false;
			}
		}
	}

	private async Task OnEnableButtonClickAsync()
	{
		try
		{
			await Task.CompletedTask;
			_sdv3.EI9 = !_sdv3.EI9;
		}
		catch
		{

		}
	}
}
