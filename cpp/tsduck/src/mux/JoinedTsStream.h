#pragma once
#include<ITSPacketConsumer.h>
#include<ITSPacketSource.h>
#include<TSPacketQueue.h>
#include<TableHandler.h>
#include<jccpp/define.h>
#include<functional>

namespace video
{
	class JoinedTsStream :public ITSPacketSource
	{
	public:
		JoinedTsStream();

	private:
		Queue<shared_ptr<ITSPacketSource>> _ts_packet_source_queue;

		/// <summary>
		///		当前正在被读取的 ITSPacketSource。
		/// </summary>
		shared_ptr<ITSPacketSource> _current_ts_packet_source;

		class TableVersionChanger;
		shared_ptr<TableVersionChanger> _table_version_changer;

	public:
		/// <summary>
		///		当一个 ITSPacketSource 对象无包可读，准备取出下一个 ITSPacketSource 时，
		///		发现储存 ITSPacketSource 对象的列表为空，就会触发此回调。
		/// 
		///		禁止将 lambda 表达式赋值给此字段，然后 lambda 表达式用值捕获指向本对象的智能指针。
		///		这会导致自引用，导致智能指针无法析构，从而导致内存泄漏。
		/// </summary>
		std::function<void()> _on_ts_packet_source_list_exhausted;

		/// <summary>
		///		从中读取包。来自不同 ITSPacketSource 的 ts 流会被拼接起来。
		///		本类会自动处理切换 ts 时的表格版本号。
		/// </summary>
		/// <param name="packet"></param>
		/// <returns></returns>
		ITSPacketSource::ReadPacketResult ReadPacket(ts::TSPacket &packet) override;

		/// <summary>
		///		向队列添加一个 ITSPacketSource 对象。不要在 _on_ts_packet_source_list_exhausted
		///		回调以外的地方调用本方法，内部队列不是线程安全的，不能边退队边入队。
		/// 
		///		送进来的 ITSPacketSource 对象的 ReadPacket 方法返回任何非 ITSPacketSource::ReadPacketResult::Success
		///		的值都会丢弃这个 ITSPacketSource，去从队列中取出下一个 ITSPacketSource。
		/// </summary>
		/// <param name="source"></param>
		void AddSource(shared_ptr<ITSPacketSource> source);

	private:
		void TryGetNextSourceIfNullAndIncreaseVersion();
	};
}