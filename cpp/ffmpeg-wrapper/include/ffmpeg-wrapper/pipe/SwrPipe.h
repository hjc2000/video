#pragma once
#include<ffmpeg-wrapper/pipe/interface/PipeFrameSource.h>
#include<SwrContextWrapper.h>

namespace video
{
	class SwrPipe :
		public PipeFrameSource,
		public IFrameConsumer
	{
	public:
		/**
		 * @brief 重采样管道。
		 * @param desired_out_frame_infos 期望输出的音频帧是什么样的
		*/
		SwrPipe(IAudioFrameInfoCollection &desired_out_frame_infos)
		{
			// 先假设输入流和输出流是一样的，随后反正可以重新构造重采样器
			_in_stream_infos = desired_out_frame_infos;
			_desired_out_frame_infos = desired_out_frame_infos;
			_swr = shared_ptr<SwrContextWrapper>{ new SwrContextWrapper{_in_stream_infos, _desired_out_frame_infos} };
		}

	private:
		shared_ptr<SwrContextWrapper> _swr;
		AudioStreamInfoCollection _in_stream_infos;
		AudioFrameInfoCollection _desired_out_frame_infos;
		AVFrameWrapper _swr_out_frame;

		/**
		 * @brief 从 _swr 中读取帧，送给消费者。
		*/
		void read_and_send_frame();

		/**
		 * @brief 这个函数是给 change_swr 函数用的。
		 *
		 * 读取并向消费者送入帧。但是遇到 eof 不会冲洗消费者。因为消费者的读取还没完，等会换个重采样器
		 * 还要继续向它们送入帧。
		*/
		void read_and_send_frame_without_flushing_consumer();

		/**
		 * @brief 输入帧的参数与 _in_stream_infos 不同。流参数发生变化了，重新构造重采样器。
		*/
		void change_swr()
		{
			cout << CODE_POS_STR << "重新构造重采样器" << endl;

			// 冲洗旧的重采样器
			_swr->SendFrame(nullptr);
			read_and_send_frame_without_flushing_consumer();

			// 构造新的重采样器
			_swr = shared_ptr<SwrContextWrapper>{ new SwrContextWrapper{_in_stream_infos, _desired_out_frame_infos} };
		}

	public:
		/// <summary>
		///		向管道送入帧。重采样后会送给消费者。如果出口处没有任何消费者，则本函数会直接返回，不执行工作。
		/// </summary>
		/// <param name="frame"></param>
		void SendFrame(AVFrameWrapper *frame) override
		{
			if (_consumer_list.Count() == 0)
			{
				return;
			}

			read_and_send_frame();
			if (frame && *frame != _in_stream_infos)
			{
				_in_stream_infos = *frame;
				change_swr();
			}

			_swr->SendFrame(frame);
			read_and_send_frame();
		}
	};
}