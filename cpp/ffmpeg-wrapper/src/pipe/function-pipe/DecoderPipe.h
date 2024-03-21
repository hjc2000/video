#pragma once
#include<AVCodecContextWrapper.h>
#include<AVStreamInfoCollection.h>
#include<AVStreamWrapper.h>
#include<ErrorCode.h>
#include<IDisposable.h>
#include<IFrameConsumer.h>
#include<IPacketConsumer.h>
#include<List.h>
#include<PipeFrameSource.h>
#include<TaskCompletionSignal.h>
#include<atomic>
#include<av_std_out.h>
#include<vector>

namespace video
{
	/**
	 * @brief 送入包，会解码然后将帧送给 IFrameConsumer。
	*/
	class DecoderPipe :
		public IPacketConsumer,
		public PipeFrameSource,
		public IDisposable,
		public IAudioStreamInfoCollection,
		public IVideoStreamInfoCollection
	{
	public:
		DecoderPipe(AVStreamInfoCollection stream)
		{
			_stream_infos = stream;
			_decoder = AVCodecContextWrapper::CreateDecoder(stream);
		}

		~DecoderPipe()
		{
			Dispose();
		}

		/**
		 * @brief 释放后 read_and_send_frame 函数内如果正在进行循环向消费者送入帧，
		 * 则会在当前循环完整后不再执行下一轮循环。
		*/
		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;
		}

	private:
		AVStreamInfoCollection _stream_infos;
		std::atomic_bool _disposed = false;
		shared_ptr<AVCodecContextWrapper> _decoder;
		AVFrameWrapper _decoder_out_frame;

		void read_and_send_frame()
		{
			while (!_disposed)
			{
				int ret = _decoder->ReadFrame(_decoder_out_frame);
				switch (ret)
				{
				case 0:
					{
						SendFrameToEachConsumer(&_decoder_out_frame);
						break;
					}
				case (int)ErrorCode::output_is_temporarily_unavailable:
					{
						// 解码器需要继续输入包才能输出帧
						return;
					}
				case (int)ErrorCode::eof:
					{
						// 解码器已被冲洗
						SendFrameToEachConsumer(nullptr);
						return;
					}
				default:
					{
						// 未知错误
						throw jc::Exception();
					}
				}
			}
		}

	public:
		/**
		 * @brief 送入包。会解码，然后将帧送给 AddFrameConsumer 添加的消费者。
		 * - 如果没有任何消费者，本函数会直接返回，不会真正去解码，这样可以节省性能。
		 * - 如果送入的包的流索引和初始化本对象时的流索引不匹配，会直接返回，不会执行解码。
		 *
		 * @param packet 要被解码的包。
		 * @exception Exception
		*/
		void SendPacket(AVPacketWrapper *packet) override
		{
			if (_consumer_list.Count() == 0)
			{
				// 管道出口没有接收者。直接返回，节省性能。
				return;
			}

			// 防止解码器中有数据残留，会导致送入包失败
			read_and_send_frame();

			if (!packet)
			{
				// 冲洗解码器
				_decoder->SendPacket(nullptr);
				read_and_send_frame();
				return;
			}

			// packet 不是空指针
			if (packet->stream_index() != _stream_infos._stream_index)
			{
				// 索引不匹配，直接返回。
				return;
			}

			_decoder->SendPacket(packet);
			read_and_send_frame();
		}

		shared_ptr<AVCodecContextWrapper> decoder_ctx()
		{
			return _decoder;
		}

		/// <summary>
		///		冲洗解码器，但是不冲洗消费者。
		/// </summary>
		void FlushDecoderButNotFlushConsumers()
		{
			_decoder->SendPacket(nullptr);
			while (!_disposed)
			{
				int ret = _decoder->ReadFrame(_decoder_out_frame);
				switch (ret)
				{
				case 0:
					{
						SendFrameToEachConsumer(&_decoder_out_frame);
						break;
					}
				case (int)ErrorCode::output_is_temporarily_unavailable:
					{
						// 解码器需要继续输入包才能输出帧
						return;
					}
				case (int)ErrorCode::eof:
					{
						// 解码器已被冲洗，但这里不冲洗消费者。
						return;
					}
				default:
					{
						// 未知错误
						throw jc::Exception();
					}
				}
			}
		}

		#pragma region 通过 IAudioStreamInfoCollection 继承
	public:
		AVRational time_base() override;
		void set_time_base(AVRational value) override;
		AVSampleFormat sample_format() override;
		void set_sample_format(AVSampleFormat value) override;
		int sample_rate() override;
		void set_sample_rate(int value) override;
		AVChannelLayout ch_layout() override;
		void set_ch_layout(AVChannelLayout value) override;
		#pragma endregion

		#pragma region 通过 IVideoStreamInfoCollection 继承
	public:
		int width() override;
		void set_width(int value) override;
		int height() override;
		void set_height(int value) override;
		AVPixelFormat pixel_format() override;
		void set_pixel_format(AVPixelFormat value) override;
		AVRational frame_rate() override;
		void set_frame_rate(AVRational value) override;
		#pragma endregion
	};
}