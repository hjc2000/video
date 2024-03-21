#pragma once
#include<AVCalculate.h>
#include<AVToString.h>
#include<AudioFrameInfoCollection.h>
#include<AudioStreamInfoCollection.h>
#include<Stream.h>
#include<VideoFrameInfoCollection.h>
#include<Wrapper.h>
#include<chrono>
#include<include_ffmpeg.h>

namespace video
{
	class ImageBuffer;

	class AVFrameWrapper :
		public Wrapper<AVFrame>,
		public IAudioFrameInfoCollection,
		public IVideoFrameInfoCollection
	{
		#pragma region 构造函数
	public:
		AVFrameWrapper()
		{
			_wrapped_obj = av_frame_alloc();
		}

		/**
		 * @brief 构造音频帧。会分配缓冲区。
		 * @param infos
		 * @param nb_samples
		*/
		AVFrameWrapper(IAudioStreamInfoCollection &infos, int nb_samples) :AVFrameWrapper()
		{
			IAudioStreamInfoCollection::operator=(infos);
			set_nb_samples(nb_samples);
			get_buffer(0);
		}

		/**
		 * @brief 构造音频帧。会分配缓冲区。
		 * @param infos
		*/
		AVFrameWrapper(IAudioFrameInfoCollection &infos) :AVFrameWrapper()
		{
			IAudioFrameInfoCollection::operator=(infos);
			get_buffer(0);
		}

		/**
		 * @brief 根据 infos 中的信息构造视频帧。会分配缓冲区。
		 * @param infos
		*/
		AVFrameWrapper(IVideoFrameInfoCollection &infos) :AVFrameWrapper()
		{
			IVideoFrameInfoCollection::operator=(infos);
			get_buffer(0);
		}

		AVFrameWrapper(AVFrameWrapper const &another) :AVFrameWrapper()
		{
			ref(another);
		}
		#pragma endregion

		#pragma region 赋值运算符
	public:
		AVFrameWrapper &operator=(AVFrameWrapper const &another)
		{
			ref(another);
			return *this;
		}

		AVFrameWrapper &operator=(AVFrameWrapper &&another)
		{
			ref(another);
			return *this;
		}
		#pragma endregion

	public:
		~AVFrameWrapper()
		{
			av_frame_free(&_wrapped_obj);
		}

	private:
		shared_ptr<ImageBuffer> _image_buf;

	public:
		void ChangeTimeBase(AVRational new_time_base)
		{
			AVRational old_time_base = _wrapped_obj->time_base;
			_wrapped_obj->time_base = new_time_base;
			_wrapped_obj->pts = ConvertTimeStamp(_wrapped_obj->pts, old_time_base, new_time_base);
			_wrapped_obj->duration = ConvertTimeStamp(_wrapped_obj->duration, old_time_base, new_time_base);
		}

		/// <summary>
		///		获取音频数据的大小。
		///		* 这里是根据采样格式，采样点个数，声道布局 计算出来的，且不考虑内存对齐。所以，
		///		  这里获得的是采样点实际使用的缓冲区大小，而不是分配堆内存时所分配的大小。
		/// </summary>
		/// <returns></returns>
		int audio_data_size();

		/// <summary>
		///		从指定位置开始将采样点设置为静音
		/// </summary>
		/// <param name="offset">从此位置（包括此位置）开始，对采样点设置静音</param>
		void mute(int offset);

		/// <summary>
		///		为此帧分配新的缓冲区。调用这个函数必须保证一些参数已经手动设置。
		///		* 对于视频帧，需要设置：像素格式、宽、高
		///		* 对于音频帧，需要设置：采样格式、采样点数量、声道布局
		/// </summary>
		/// <param name="align">
		///		内存对齐的尺寸。传入 0，会自动根据 CPU 选择对齐方式。如果你不知道你在干什么，就传入 0.
		/// </param>
		void get_buffer(int align);

		void make_writable();

		/// <summary>
		///		判断此帧是否可写
		/// </summary>
		/// <returns></returns>
		bool is_writable()
		{
			return av_frame_is_writable(_wrapped_obj);
		}

		/**
		 * @brief 让本帧引用另一个帧的缓冲区并复制其它参数。
		 * - 在引用另一个帧之前会先调用 unref 方法。
		 *
		 * @param another
		*/
		void ref(AVFrameWrapper const &other)
		{
			unref();
			int ret = av_frame_ref(_wrapped_obj, (AVFrameWrapper &)other);
			if (ret < 0)
			{
				throw jc::Exception();
			}
		}

		/**
		 * @brief 解除此帧对缓冲区的引用。重复调用不会出错
		*/
		void unref()
		{
			::av_frame_unref(_wrapped_obj);
		}

	public:// 时间
		int64_t pts()
		{
			return _wrapped_obj->pts;
		}
		void set_pts(int64_t value)
		{
			_wrapped_obj->pts = value;
		}

		int64_t duration()
		{
			return _wrapped_obj->duration;
		}
		void set_duration(int64_t value)
		{
			_wrapped_obj->duration = value;
		}

		/// <summary>
		///		计算本帧的 pts 所对应的时间。
		///		* 需要保证本帧的时间基被正确设置。
		/// </summary>
		/// <returns></returns>
		std::chrono::milliseconds PtsToMilliseconds()
		{
			int64_t num = pts() * 1000 * time_base().num;
			int64_t den = time_base().den;
			std::chrono::milliseconds m{ num / den };
			return m;
		}

	public:// 复制数据到流，缓冲区
		void copy_image_to_buffer(shared_ptr<ImageBuffer> buffer);

		/// <summary>
		///		将视频帧复制到流中.请确保本帧是视频帧。
		/// </summary>
		/// <param name="stream"></param>
		void CopyVideoFrameToStream(Stream &stream);

		/// <summary>
		///		将音频帧复制到流中。请确保本帧是音频帧。
		///		注意：
		///		* 调用本方法请保证帧的格式是交织的而非平面的。例如写入文件流，作为 PCM 文件，PCM
		///		  文件要求不同声道的采样点必须是交织存放的。
		/// </summary>
		/// <param name="stream">要复制到的流</param>
		void CopyAudioFrameToStream(Stream &stream);

		/// <summary>
		///		将音频数据复制到缓冲区中。要求本帧的音频数据是交错类型的。
		/// </summary>
		/// <param name="buffer"></param>
		/// <param name="len"></param>
		void CopyAudioDataToBuffer(uint8_t *buffer, int len)
		{
			if (IsPlanar())
			{
				throw jc::Exception("本帧是平面类型，写入缓冲区的音频数据不允许是平面类型");
			}

			memcpy(buffer, _wrapped_obj->extended_data[0], len);
		}

		std::string ToString()
		{
			return std::format(
				"pts={}, time_base={}, sample_format={}",
				_wrapped_obj->pts,
				::ToString(_wrapped_obj->time_base),
				!_wrapped_obj->width ? ::ToString(sample_format()) : ""
			);
		}

		#pragma region IAudioFrameInfoCollection
		AVSampleFormat sample_format() override;
		void set_sample_format(AVSampleFormat value) override;

		int nb_samples() override;
		void set_nb_samples(int value) override;

		AVChannelLayout ch_layout() override;
		void set_ch_layout(AVChannelLayout value) override;

		int sample_rate() override;
		void set_sample_rate(int value) override;

		/// <summary>
		///		AVFrameWrapper 的时间基属性不一定是有效的。例如从重采样器或解码器
		///		中输出的 AVFrameWrapper 的时间基属性就是无效值。
		/// </summary>
		/// <returns></returns>
		AVRational time_base() override;
		void set_time_base(AVRational value) override;
		#pragma endregion

		#pragma region IVideoFrameInfoCollection
		int width() override;
		void set_width(int value) override;

		int height() override;
		void set_height(int value) override;

		AVPixelFormat pixel_format() override;
		void set_pixel_format(AVPixelFormat value) override;
		#pragma endregion
	};
}
