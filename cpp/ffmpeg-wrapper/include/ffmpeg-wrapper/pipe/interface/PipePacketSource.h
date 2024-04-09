#pragma once
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<jccpp/container/List.h>

namespace video
{
	class IPipePacketSource
	{
	public:
		virtual ~IPipePacketSource() {}

	public:
		virtual void AddPacketConsumer(shared_ptr<IPacketConsumer> packet_comsumer) = 0;
		virtual bool RemovePacketConsumer(shared_ptr<IPacketConsumer> packet_comsumer) = 0;
		virtual void ClearPacketConsumer() = 0;
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
		bool RemovePacketConsumer(shared_ptr<IPacketConsumer> packet_comsumer) override;
		void ClearPacketConsumer() override;

		/// <summary>
		///		将另一个 PipePacketSource 的消费者复制过来。
		/// </summary>
		/// <param name="another"></param>
		void AddConsumerFromAnotherPipe(PipePacketSource &another);
	};
}