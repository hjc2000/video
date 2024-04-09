#pragma once
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<jccpp/container/List.h>

namespace video
{
	class IPipeFrameSource
	{
	public:
		virtual ~IPipeFrameSource() {}

	public:
		virtual void AddFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer) = 0;
		void AddFrameConsumer(List<shared_ptr<IFrameConsumer>> frame_consumers);
		virtual bool RemoveFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer) = 0;
		virtual void ClearFrameConsumer() = 0;
	};

	class PipeFrameSource :public IPipeFrameSource
	{
	public:
		virtual ~PipeFrameSource() {}

	protected:
		List<shared_ptr<IFrameConsumer>> _consumer_list;

		/// <summary>
		///		向每个消费者送入帧。
		/// </summary>
		void SendFrameToEachConsumer(AVFrameWrapper *frame);

	public:
		using IPipeFrameSource::AddFrameConsumer;
		void AddFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer) override;
		bool RemoveFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer) override;
		void ClearFrameConsumer() override;

		/// <summary>
		///		将另一个 PipeFrameSource 的消费者复制过来
		/// </summary>
		/// <param name="another"></param>
		void AddConsumerFromAnotherPipe(PipeFrameSource &another);
	};
}