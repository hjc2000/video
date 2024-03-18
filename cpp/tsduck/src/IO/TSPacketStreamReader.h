#include<Stream.h>

namespace video
{
	/// <summary>
	///		用于从流中读取 ts 包。
	/// </summary>
	class TSPacketStreamReader
	{
	public:
		TSPacketStreamReader(shared_ptr<Stream> input_stream)
		{
			_input_stream = input_stream;
		}

	private:
		shared_ptr<Stream> _input_stream;
	};
}