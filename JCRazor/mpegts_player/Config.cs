using System.Text.Json.Serialization;

namespace JCRazor.mpegts_player;
public struct Config
{
	public Config() { }

	[JsonPropertyName("enableWorker")]
	public bool EnableWorker { get; set; } = true;

	[JsonPropertyName("enableWorkerForMSE")]
	public bool EnableWorkerForMSE { get; set; } = true;

	[JsonPropertyName("enableStashBuffer")]
	public bool EnableStashBuffer { get; set; } = false;

	[JsonPropertyName("stashInitialSize")]
	public double StashInitialSize { get; set; } = 64;

	[JsonPropertyName("liveBufferLatencyChasing")]
	public bool LiveBufferLatencyChasing { get; set; } = false;

	[JsonPropertyName("liveBufferLatencyChasingOnPaused")]
	public bool? LiveBufferLatencyChasingOnPaused { get; set; } = null;

	[JsonPropertyName("liveBufferLatencyMaxLatency")]
	public double? LiveBufferLatencyMaxLatency { get; set; } = null;

	[JsonPropertyName("liveBufferLatencyMinRemain")]
	public double? LiveBufferLatencyMinRemain { get; set; } = null;

	[JsonPropertyName("liveSync")]
	public bool LiveSync { get; set; } = false;

	[JsonPropertyName("liveSyncMaxLatency")]
	public double? LiveSyncMaxLatency { get; set; } = null;

	[JsonPropertyName("liveSyncTargetLatency")]
	public double? LiveSyncTargetLatency { get; set; } = null;

	[JsonPropertyName("liveSyncPlaybackRate")]
	public double? LiveSyncPlaybackRate { get; set; } = null;

	/// <summary>
	///		如果有足够的视频数据可以播放，中断 http 连接。
	/// </summary>
	[JsonPropertyName("lazyLoad")]
	public bool LazyLoad { get; set; } = false;

	[JsonPropertyName("deferLoadAfterSourceOpen")]
	public bool? DeferLoadAfterSourceOpen { get; set; } = null;

	[JsonPropertyName("autoCleanupSourceBuffer")]
	public bool AutoCleanupSourceBuffer { get; set; } = true;

	[JsonPropertyName("autoCleanupMaxBackwardDuration")]
	public double AutoCleanupMaxBackwardDuration { get; set; } = 20;

	[JsonPropertyName("autoCleanupMinBackwardDuration")]
	public double AutoCleanupMinBackwardDuration { get; set; } = 10;

	/// <summary>
	///		修复大的音频间隙。会填充静音帧。
	/// </summary>
	[JsonPropertyName("fixAudioTimestampGap")]
	public bool FixAudioTimestampGap { get; set; } = false;

	[JsonPropertyName("accurateSeek")]
	public bool? AccurateSeek { get; set; } = null;

	/// <summary>
	///		可以选择 range 或 param。
	/// </summary>
	[JsonPropertyName("seekType")]
	public string SeekType { get; set; } = "range";

	[JsonPropertyName("seekParamStart")]
	public string? SeekParamStart { get; set; } = null;

	[JsonPropertyName("seekParamEnd")]
	public string? SeekParamEnd { get; set; } = null;

	[JsonPropertyName("rangeLoadZeroStart")]
	public bool? RangeLoadZeroStart { get; set; } = null;
}
