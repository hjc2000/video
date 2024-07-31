using Microsoft.AspNetCore.Components;
using RazorUI.导航;
using System.Threading.Tasks;

namespace Sdv3ControlPanel.Pages;

public partial class CatchAllPage
{
	protected override async Task OnParametersSetAsync()
	{
		await base.OnParametersSetAsync();
		Redirector.Redirect();
	}

	[Parameter]
	public string Path { get; set; } = string.Empty;

	[Inject]
	private Redirector Redirector { get; set; } = default!;
}
