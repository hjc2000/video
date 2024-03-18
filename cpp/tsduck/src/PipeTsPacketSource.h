#pragma once
#include<ITSPacketConsumer.h>
#include<List.h>
#include<define.h>
#include<memory>
#include<tsTSPacket.h>

using std::shared_ptr;

namespace video
{
	class IPipeTsPacketSource
	{
	public:
		virtual ~IPipeTsPacketSource() {}

	public:
		/// <summary>
		///		添加一个包消费者
		/// </summary>
		/// <param name="packet_comsumer"></param>
		virtual void AddTsPacketConsumer(shared_ptr<ITsPacketConsumer> packet_comsumer) = 0;

		/// <summary>
		///		移除指定的包消费者。
		/// </summary>
		/// <param name="packet_comsumer"></param>
		/// <returns>容器中存在该消费者且移除成功则返回 true，否则返回 false。</returns>
		virtual bool RemovePacketComsumer(shared_ptr<ITsPacketConsumer> packet_comsumer) = 0;

		/// <summary>
		///		清空所有消费者。
		/// </summary>
		virtual void ClearConsumers() = 0;
	};

	class PipeTsPacketSource :public IPipeTsPacketSource
	{
	protected:
		List<shared_ptr<ITsPacketConsumer>> _consumer_list;

		void SendPacketToEachConsumer(ts::TSPacket *packet);
		void SendPacketToEachConsumer(std::vector<ts::TSPacket> packets);
		void SendPacketToEachConsumer(std::vector<std::vector<ts::TSPacket>> packet_vectors);

	public:
		virtual void AddTsPacketConsumer(shared_ptr<ITsPacketConsumer> packet_comsumer) override;
		virtual bool RemovePacketComsumer(shared_ptr<ITsPacketConsumer> packet_comsumer) override;
		virtual void ClearConsumers() override;

	public:
		void AddTsPacketConsumerFromAnother(PipeTsPacketSource &another);
	};
}