namespace JCRazor.mpegts_player;

/// <summary>
///		MpegTSPlayer 的 ts 流源。MpegTSPlayer 将从此对象获取 Stream。
///		Stream 中传输的是 ts 的字节流。
/// </summary>
public interface ITSStreamSource
{
	Task<Stream?> GetTSStreamAsync(CancellationToken cancellationToken);
}
