using libsdv3.Modbus;
using System;
using System.IO.Ports;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class TestPage : IAsyncDisposable
{
	#region 生命周期
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
		_serial?.Dispose();
	}
	#endregion

	private ModbusSdv3Device? _sdv3;
	private SerialPort? _serial;

	private async Task OnConnecteAsync(SerialPort serial)
	{
		await Task.CompletedTask;
		_serial = serial;
		_serial.Open();
		_sdv3 = new ModbusSdv3Device(_serial.BaseStream, 1, true);
		//JCNET.定时器.Timer.SetInterval(() =>
		//{

		//},
		//1000,
		//CancellationToken.None);
	}

	private bool Enabled
	{
		get
		{
			try
			{
				if (_sdv3 is null)
				{
					return false;
				}

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
			if (_sdv3 is null)
			{
				return;
			}

			await Task.CompletedTask;
			_sdv3.EI9 = !_sdv3.EI9;
		}
		catch
		{

		}
	}

	private uint FeedbackCurrentPosition
	{
		get
		{
			try
			{
				if (_sdv3 is null)
				{
					return 0;
				}

				return _sdv3.FeedbackCurrentPosition;
			}
			catch
			{
				return 0;
			}
		}
	}
}
