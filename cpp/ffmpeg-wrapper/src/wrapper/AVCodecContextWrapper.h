#pragma once
#include<AVCodecExtention.h>
#include<AVStreamInfoCollection.h>
#include<AVStreamWrapper.h>
#include<IAudioFrameInfoCollection.h>
#include<IFrameConsumer.h>
#include<IFrameSource.h>
#include<IPacketConsumer.h>
#include<IVideoStreamInfoCollection.h>
#include<Wrapper.h>
#include<include_ffmpeg.h>
#include<include_ffmpeg.h>

namespace video
{
	class AVFrameWrapper;
	class AVPacketWrapper;

	/// <summary>
	///		* 本类通过工厂函数创建，构造函数都是私有的。
	///		* 本类线程安全，所有公共方法都有加锁。
	/// </summary>
	class AVCodecContextWrapper :
		public Wrapper<AVCodecContext>,
		public IAudioStreamInfoCollection,
		public IVideoStreamInfoCollection,
		public IFrameSource,
		public IPacketConsumer
	{
	private:
		AVCodecContextWrapper(AVCodec const *codec);
		AVCodecContextWrapper(AVCodec const *codec, AVCodecParameters *param);

	public:
		~AVCodecContextWrapper();

		/// <summary>
		///		创建解码器。此函数创建的解码器是通用的，音频流和视频流的解码器都是用这个函数创建。
		///		* 会将流的 AVCodecParameters 复制到解码器上下文中
		///		* 会将流的时间基复制到解码器上下文中。
		///		* 会将流的帧率复制到解码器上下文中。（虽然帧率只对视频流有效，但音频流反正不会用到，复制了也没事）
		///		* 返回的解码器将处于打开状态
		/// </summary>
		/// <param name="stream"></param>
		/// <returns></returns>
		static shared_ptr<AVCodecContextWrapper> CreateDecoder(AVStreamInfoCollection stream);

		/// <summary>
		///		创建音频编码器
		/// </summary>
		/// <param name="encoder_name">通过编码器的名称查找编码器</param>
		/// <param name="infos">
		///		音频流信息。将会利用此流的信息创建编码器。
		///		* 所利用的信息包括时间基。
		/// </param>
		/// <param name="set_global_header">
		///		是否需要设置全局头部。
		///		* 某些封装格式要求编码器设置全局头部。可以通过检查 AVFormatContextWrapper 的
		///		  NeedGlobalHeader 属性来确定是否需要全局头部。
		/// </param>
		/// <returns></returns>
		static shared_ptr<AVCodecContextWrapper> CreateEncoder(
			const char *encoder_name,
			IAudioStreamInfoCollection &infos,
			bool set_global_header,
			bool auto_open = true
		);

		/**
		* @brief 创建视频编码器。返回的编码器已打开。
		* @param encoder_name 通过编码器的名称查找编码器
		* @param infos 视频流信息。将会利用此流的信息创建编码器。所利用的信息包括时间基。
		*
		* @param set_global_header 是否需要设置全局头部。
		* * 某些封装格式要求编码器设置全局头部。可以通过检查 AVFormatContextWrapper 的
		* NeedGlobalHeader 属性来确定是否需要全局头部。
		*
		* @return 返回的编码器已打开。
		*/
		static shared_ptr<AVCodecContextWrapper> CreateEncoder(
			const char *encoder_name,
			IVideoStreamInfoCollection &infos,
			bool set_global_header,
			bool auto_open = true
		);

	private:
		#pragma region 码器
		/// <summary>
		/// 与本类绑定的 AVCodecWrapper
		/// </summary>
		AVCodec *_codec = nullptr;

		AVCodec const *Codec() const
		{
			return _codec;
		}

		void SetCodec(AVCodec const *codec)
		{
			_codec = (AVCodec *)codec;
		}
		#pragma endregion

	public:
		/**
		 * @brief 打开编解码器。
		 * 注意，打开前一定要先设置编解码器的参数。可以使用 set_encoder_param 函数设置，也可以直接用
		 * AVCodecContextWrapper(const AVCodec *codec, AVCodecParameters *param, bool auto_open_codec = false);
		 * 构造一个已经设置好参数的编解码器。
		 *
		 * @param dic
		*/
		void Open(AVDictionary **dic = nullptr)
		{
			int ret = ::avcodec_open2(_wrapped_obj, _codec, dic);
			if (ret)
			{
				std::cerr << CODE_POS_STR << "打开编解码器失败" << endl;
				throw jc::Exception();
			}
		}

		/**
		 * @brief 设置此码器上下文的参数。调用此方法不会设置时间基，需要另外手动设置。
		 * @param param
		*/
		void SetCodecParam(AVCodecParameters *param);

		/**
		 * @brief 设置全局头部。
		 * 某些封装格式要求编码器在编码的时候设置全局头部。
		 * 要知道封装格式是不是需要设置全局头部，可以查看 AVFormatContextWrapper 的 NeedGlobalHeader
		 * 属性。
		*/
		void SetGlobalHeader()
		{
			_wrapped_obj->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		}

		/**
		 * @brief 将未编码帧送入编码器进行编码。随后可调用 read_packet 方法接收编码完成的包。
		 * - 送入的帧的 pts 会被缩放为本对象时间基上的 pts，然后将帧的时间基设置为本对象的时间基。
		 *
		 * @param frame 要被编码的帧。可以送入空指针，此时表示冲洗编码器。
		 * @return 成功返回 0，失败返回错误代码
		 *
		 * @exception SendFrameException
		*/
		void SendFrame(AVFrameWrapper *frame);
		bool _have_printed_send_frame_rescale_pts_warning = false;

		/**
		 * @brief 接收包
		 * - AVCodecContext 输出的包不带时间基，只带时间戳。但本函数会补充设置时间基，
		 * 将包的时间基设置为本对象的时间基。
		 *
		 * @param packet
		 * @return
		*/
		int ReadPacket(AVPacketWrapper &packet);

		/**
		 * @brief 向编码器发送包
		 * @param packet 要被解码的包。可以传入空指针，此时表示冲洗解码器。
		 * @exception SendPacketException 送入包失败会抛出异常
		*/
		void SendPacket(AVPacketWrapper *packet) override;

		/**
		 * @brief 接收解码后的帧
		 * - 在实际接收帧之前，此帧如果有引用缓冲区，会首先解除对缓冲区的引用。
		 * - 接收到的帧的时间基会被赋值为本对象的时间基。
		 *
		 * @param frame 用来接收数据的帧
		 * @return 接收成功返回 0，失败返回错误代码。
		*/
		int ReadFrame(AVFrameWrapper &frame) override;

		#pragma region IAudioStreamInfoCollection
		AVChannelLayout ch_layout() override
		{
			return _wrapped_obj->ch_layout;
		}

		void set_ch_layout(AVChannelLayout value) override
		{
			_wrapped_obj->ch_layout = value;
		}

		AVSampleFormat sample_format() override
		{
			return _wrapped_obj->sample_fmt;
		}

		void set_sample_format(AVSampleFormat value) override
		{
			_wrapped_obj->sample_fmt = value;
		}

		int sample_rate() override
		{
			return _wrapped_obj->sample_rate;
		}

		void set_sample_rate(int value) override
		{
			_wrapped_obj->sample_rate = value;
		}
		#pragma endregion

		#pragma region IVideoStreamInfoCollection
		int width() override
		{
			return _wrapped_obj->width;
		}
		void set_width(int value) override
		{
			_wrapped_obj->width = value;
		}

		int height() override
		{
			return _wrapped_obj->height;
		}
		void set_height(int value) override
		{
			_wrapped_obj->height = value;
		}

		AVPixelFormat pixel_format() override
		{
			return _wrapped_obj->pix_fmt;
		}

		void set_pixel_format(AVPixelFormat value) override
		{
			_wrapped_obj->pix_fmt = value;
		}

		/**
		 * @brief 获取此码器的时间基。作为编码器，时间基必须由用户手动设置，作为解码器，时间基是
		 * 无用的，无效的，不要试图从解码器中获取此参数。
		 * @return
		*/
		AVRational time_base() override
		{
			return _wrapped_obj->time_base;
		}

		void set_time_base(AVRational value) override
		{
			_wrapped_obj->time_base = value;
		}

		AVRational frame_rate() override
		{
			return _wrapped_obj->framerate;
		}

		void set_frame_rate(AVRational value) override
		{
			_wrapped_obj->framerate = value;
		}
		#pragma endregion

	};
}
