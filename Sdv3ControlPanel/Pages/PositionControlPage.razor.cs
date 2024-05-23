using Sdv3ControlPanel.Data;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class PositionControlPage
{
	private async Task ConfigAsync()
	{
		if (Database.SDV3 is null)
		{
			return;
		}

		// 定位运行模式
		await Database.SDV3.SetPnAsync(1, 1, 7);

		// 内部定位数据无效
		await Database.SDV3.SetPnAsync(2, 40, 0);

		// 将 EI9 设置为定位数据启动
		await Database.SDV3.SetPnAsync(3, 9, 4);
	}
}
