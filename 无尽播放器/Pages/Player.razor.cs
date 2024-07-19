using RazorUI.ts播放器;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace 无尽播放器.Pages;
public partial class Player : ITSStreamSource
{
	public async Task<Stream?> GetTSStreamAsync(CancellationToken cancellationToken)
	{
		await Task.CompletedTask;
		FileStream fs = File.Open(
		   "D:/repos/video/cpp/test/资源/mux_out.ts",
		   FileMode.Open,
		   FileAccess.Read,
		   FileShare.ReadWrite
		);
		return fs;
	}
}
