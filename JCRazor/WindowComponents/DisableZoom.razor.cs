using Microsoft.JSInterop;

namespace JCRazor.WindowComponents;

public partial class DisableZoom
{
	private IJSObjectReference moudel = default!;
	protected override async Task OnInitializedAsync()
	{
		moudel = await JS.InvokeAsync<IJSObjectReference>("import",
		"./_content/JCRazor/WindowComponents/DisableZoom.razor.js");
		await moudel.InvokeVoidAsync("DisableZoom");
	}
}
