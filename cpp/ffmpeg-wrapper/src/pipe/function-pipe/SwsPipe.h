#pragma once
#include<IFrameConsumer.h>
#include<PipeFrameSource.h>
#include<SwsContextWrapper.h>
#include<VideoFrameInfoCollection.h>

namespace video
{
	class SwsPipe :
		public PipeFrameSource,
		public IFrameConsumer
	{
	public:
		SwsPipe(IVideoFrameInfoCollection &desire_out_video_frame_infos)
		{
			_in_video_frame_infos = desire_out_video_frame_infos;
			_desire_out_video_frame_infos = desire_out_video_frame_infos;

			_sws_context = shared_ptr<SwsContextWrapper>{
				new SwsContextWrapper {
					_in_video_frame_infos,
					_desire_out_video_frame_infos
				}
			};
		}

	private:
		shared_ptr<SwsContextWrapper> _sws_context;
		VideoFrameInfoCollection _in_video_frame_infos;
		VideoFrameInfoCollection _desire_out_video_frame_infos;
		AVFrameWrapper _sws_out_frame;

		void read_and_send_frame_to_consumer()
		{
			// 因为 sws 内没有比较深的队列，它只会储存 1 帧，所以不用在循环里 read_frame
			int ret = _sws_context->ReadFrame(_sws_out_frame);
			switch (ret)
			{
			case 0:
				{
					SendFrameToEachConsumer(&_sws_out_frame);
					_sws_out_frame.unref();
					break;
				}
			case (int)ErrorCode::output_is_temporarily_unavailable:
				{
					return;
				}
			case (int)ErrorCode::eof:
				{
					SendFrameToEachConsumer(nullptr);
					return;
				}
			}
		}

		void change_sws()
		{
			cout << CODE_POS_STR << "重新构造 sws。" << endl;
			read_and_send_frame_to_consumer();
			_sws_context = shared_ptr<SwsContextWrapper>{
				new SwsContextWrapper {
					_in_video_frame_infos,
					_desire_out_video_frame_infos
				}
			};
		}

	public:
		void SendFrame(AVFrameWrapper *frame) override;
	};
}