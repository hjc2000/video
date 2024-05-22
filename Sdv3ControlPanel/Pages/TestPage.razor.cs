using JCRazor.表单;
using Sdv3ControlPanel.Data;
using System;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class TestPage : IAsyncDisposable
{
	protected override async Task OnInitializedAsync()
	{
		await base.OnInitializedAsync();
		Database.DataUpdatedEvent += OnDataUpdatedEvent;
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

		await Task.CompletedTask;
		Database.DataUpdatedEvent -= OnDataUpdatedEvent;
	}

	private async void OnDataUpdatedEvent()
	{
		await InvokeAsync(StateHasChanged);
	}

	/// <summary>
	///		连接按钮点击事件处理函数
	/// </summary>
	/// <param name="serial_port_options"></param>
	/// <returns></returns>
	private async Task OnConnecteButtonClickAsync(SerialPortOptions serial_port_options)
	{
		await Database.ConnectAsync(serial_port_options);
	}

	/// <summary>
	///		使能按钮点击事件处理函数
	/// </summary>
	/// <returns></returns>
	private async Task OnEnableButtonClickAsync()
	{
		try
		{
			if (Database.SDV3 is null)
			{
				return;
			}

			Database.LogOutputPort.WriteLine("使能按钮处理函数");
			await Database.SDV3.SetEI9Async(!await Database.SDV3.GetEI9Async());
			await Database.SDV3.SetEI10Async(true);
		}
		catch (Exception ex)
		{
			Database.LogOutputPort.WriteLine(ex.ToString());
		}
	}

	/// <summary>
	///		正转按钮点击事件处理函数
	/// </summary>
	/// <returns></returns>
	private async Task OnFwdClickAsync()
	{
		try
		{
			if (Database.SDV3 is null)
			{
				return;
			}

			Database.LogOutputPort.WriteLine("正转按钮处理函数");
			await Database.SDV3.SetEI11Async(!await Database.SDV3.GetEI11Async());
		}
		catch (Exception ex)
		{
			Database.LogOutputPort.WriteLine(ex.ToString());
		}
	}
}
