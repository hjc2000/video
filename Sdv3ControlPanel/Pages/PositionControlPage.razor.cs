using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class PositionControlPage
{
	private async Task ConfigAsync()
	{
		await Task.CompletedTask;
		//await Database.SetP1_01Async(7);// 定位运行模式
		//await Database.SetP2_40Async(0);// 内部定位数据无效
		//await Database.SetP3_09Async(4);// 将 EI9 设置为定位数据启动
	}
}
