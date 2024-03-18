#pragma once
#include<IPacketConsumer.h>
#include<List.h>

namespace video
{
	class IPipePacketSource
	{
	public:
		virtual ~IPipePacketSource() {}

	public:
		virtual void AddPacketConsumer(shared_ptr<IPacketConsumer> packet_comsumer) = 0;
		virtual bool RemovePacketComsumer(shared_ptr<IPacketConsumer> packet_comsumer) = 0;
	};

	class PipePacketSource :public IPipePacketSource
	{
	public:
		virtual ~PipePacketSource() {}

	protected:
		List<shared_ptr<IPacketConsumer>> _consumer_list;

		/// <summary>
		///		向每个消费者送入包
		/// </summary>
		/// <param name="packet"></param>
		void SendPacketToEachConsumer(AVPacketWrapper *packet);

	public:
		void AddPacketConsumer(shared_ptr<IPacketConsumer> packet_comsumer) override;

		bool RemovePacketComsumer(shared_ptr<IPacketConsumer> packet_comsumer) override;

		/// <summary>
		///		将另一个 PipePacketSource 的消费者复制过来。
		/// </summary>
		/// <param name="another"></param>
		void AddConsumerFromAnotherPipe(PipePacketSource &another);
	};
}