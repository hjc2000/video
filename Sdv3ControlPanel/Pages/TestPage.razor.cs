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

		if (_sdv3 is not null)
		{
			await _sdv3.DisposeAsync();
		}
	}
	#endregion

	private ModbusSdv3Device? _sdv3;

	private async Task OnConnecteButtonClickAsync(SerialPort serial)
	{
		try
		{
			if (_sdv3 is not null)
			{
				await _sdv3.DisposeAsync();
			}

			serial.ReadTimeout = 2000;
			serial.WriteTimeout = 2000;
			await Task.Run(serial.Open);
			_sdv3 = new ModbusSdv3Device(serial.BaseStream, 1, true);
			//JCNET.定时器.Timer.SetInterval(() =>
			//{

			//},
			//1000,
			//CancellationToken.None);
		}
		catch
		{

		}
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
