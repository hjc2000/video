namespace JCNET.流.SerialFrame;
public class SerialFrameDecoder
{
	/// <summary>
	/// 
	/// </summary>
	/// <param name="src_stream">数据源。从这里读出数据并解码，还原出原始数据。</param>
	public SerialFrameDecoder(Stream src_stream)
	{
		_src_stream = src_stream;
	}

	/// <summary>
	///		数据源。从这里读出数据并解码，还原出原始数据。
	/// </summary>
	private Stream _src_stream;
}
