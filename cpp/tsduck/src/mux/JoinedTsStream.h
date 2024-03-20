#include<ITSPacketConsumer.h>
#include<ITSPacketSource.h>
#include<functional>
#include<vector>

namespace video
{
	class JoinedTsStream
	{
	public:
		JoinedTsStream() {}

		/// <summary>
		///		从 ts_packet_sources 中取出 ITSPacketSource 对象，从中读取 ts 包。
		///		一个 ITSPacketSource 对象的 ts 包读完了就取出下一个继续读取，直到
		///		ts_packet_sources 耗尽。
		/// </summary>
		/// <param name="ts_packet_sources"></param>
		JoinedTsStream(std::vector<shared_ptr<ITSPacketSource>> ts_packet_sources)
		{
			_ts_packet_sources = ts_packet_sources;
		}

	private:
		std::vector<shared_ptr<ITSPacketSource>> _ts_packet_sources;

	public:
		/// <summary>
		///		当一个 ITSPacketSource 对象无包可读，准备取出下一个 ITSPacketSource 时，
		///		发现储存 ITSPacketSource 对象的列表为空，就会触发此回调。
		/// 
		///		禁止将 lambda 表达式赋值给此字段，然后 lambda 表达式用值捕获指向本对象的智能指针。
		///		这会导致自引用，导致智能指针无法析构，从而导致内存泄漏。
		/// </summary>
		std::function<void(void)> _on_ts_packet_source_list_exhausted;
	};
}
