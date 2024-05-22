using Sdv3ControlPanel.Data;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class PositionControlPage
{
	private async Task ConfigAsync()
	{
		await Database.SetP1_01Async(7);
	}
}
