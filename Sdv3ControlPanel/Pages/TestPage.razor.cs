﻿using libsdv3.Modbus;
using System;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class TestPage : IAsyncDisposable
{
	#region 生命周期
	protected override async Task OnAfterRenderAsync(bool firstRender)
	{
		await base.OnAfterRenderAsync(firstRender);
		if (firstRender)
		{
			JCNET.定时器.Timer.SetInterval(async () =>
			{
				await InvokeAsync(StateHasChanged);
			},
			1000,
			_cancel_timer.Token);
		}
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

		if (_sdv3 is not null)
		{
			await _sdv3.DisposeAsync();
		}

		_cancel_timer.Cancel();
	}
	#endregion

	private ModbusSdv3Device? _sdv3;
	private CancellationTokenSource _cancel_timer = new();

	/// <summary>
	///		连接按钮点击事件
	/// </summary>
	/// <param name="serial"></param>
	/// <returns></returns>
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

	private int FeedbackCurrentPosition
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