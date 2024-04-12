#pragma once
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<jccpp/container/List.h>

namespace video
{
	class IPipeFrameSource
	{
	public:
		virtual ~IPipeFrameSource() = default;

	public:
		virtual List<shared_ptr<IFrameConsumer>> &ConsumerList() = 0;

		/// <summary>
		///		向每个消费者送入帧。
		/// </summary>
		void SendFrameToEachConsumer(AVFrameWrapper *frame)
		{
			for (shared_ptr<IFrameConsumer> &consumer : ConsumerList())
			{
				consumer->SendFrame(frame);
			}
		}

		void AddFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer)
		{
			if (!frame_consumer)
			{
				cout << CODE_POS_STR << "添加帧消费者时传入了空指针" << endl;
				return;
			}

			ConsumerList().Add(frame_consumer);
		}

		void AddFrameConsumer(List<shared_ptr<IFrameConsumer>> frame_consumers)
		{
			for (auto consumer : frame_consumers)
			{
				AddFrameConsumer(consumer);
			}
		}

		bool RemoveFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer)
		{
			if (!frame_consumer)
			{
				return false;
			}

			return ConsumerList().Remove(frame_consumer);
		}

		void ClearFrameConsumer()
		{
			ConsumerList().Clear();
		}
	};
}