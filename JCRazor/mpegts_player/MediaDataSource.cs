using System.Text.Json.Serialization;

namespace JCRazor.mpegts_player;

public struct MediaDataSource
{
	public MediaDataSource() { }

	public MediaDataSource(string url, bool is_live)
	{
		Url = url;
		IsLive = is_live;
	}

	[JsonPropertyName("type")]
	public string Type { get; set; } = "mpegts";

	[JsonPropertyName("isLive")]
	public bool IsLive { get; set; } = true;

	[JsonPropertyName("cors")]
	public bool Cors { get; set; } = true;

	[JsonPropertyName("withCredentials")]
	public bool? WithCookies { get; set; } = null;

	[JsonPropertyName("hasVideo")]
	public bool HasVideo { get; set; } = true;

	[JsonPropertyName("hasAudio")]
	public bool HasAudio { get; set; } = true;

	[JsonPropertyName("duration")]
	public double? Duration { get; set; } = null;

	[JsonPropertyName("filesize")]
	public double? FileSize { get; set; } = null;

	[JsonPropertyName("url")]
	public string? Url { get; set; } = null;
}
