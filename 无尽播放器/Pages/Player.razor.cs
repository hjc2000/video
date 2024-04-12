using JCRazor.mpegts_player;
using PInvokeLib;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace 无尽播放器.Pages;
public partial class Player : ITSStreamSource
{
	public async Task<Stream?> GetTSStreamAsync(CancellationToken cancellationToken)
	{
		await Task.CompletedTask;
		CppStream stream = CppStream.GetFileStream("");
		return stream;
	}
}
