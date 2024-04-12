#pragma once
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include<ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>

namespace video
{
	/// <summary>
	///		AVStream 会随着 AVFormatContex 的销毁而销毁，所以不能长久保留 AVStream 的指针，
	///		这会导致访问失效内存。所以设计了本类，用来储存 AVStream 中的信息。这是基于值的复制，
	///		深度拷贝。
	/// </summary>
	class AVStreamInfoCollection :
		public IAudioStreamInfoCollection,
		public IVideoStreamInfoCollection
	{
	public:
		AVStreamInfoCollection() {}
		AVStreamInfoCollection(AVStreamInfoCollection const &stream);
		AVStreamInfoCollection(AVStreamWrapper const &stream);
		~AVStreamInfoCollection();

		#pragma region 赋值运算符
	public:
		AVStreamInfoCollection &operator=(AVStreamInfoCollection const &stream);
		AVStreamInfoCollection &operator=(AVStreamInfoCollection &stream);

		AVStreamInfoCollection &operator=(AVStreamWrapper const &stream);
		AVStreamInfoCollection &operator=(AVStreamWrapper &stream);
		#pragma endregion

		/// <summary>
		///		当且仅当两个对象的指针相等时才相等。
		/// </summary>
		/// <param name="another"></param>
		/// <returns></returns>
		bool operator==(AVStreamInfoCollection const &another)
		{
			return this == &another;
		}

	private:
		void CopyCodecParamFrom(AVCodecParameters *src);

	public:
		int _index = -1;
		int Index();
		void SetIndex(int value);

		AVMediaType _media_type{};
		int64_t _bitrate = 0;
		AVCodecParameters *_codec_params = avcodec_parameters_alloc();
		AVCodec const *_codec = nullptr;

		AVRational _timebase{};
		AVSampleFormat _sample_format{};
		int _sample_rate = 0;
		AVChannelLayout _ch_layout{};
		int _width = 0;
		int _height = 0;
		AVPixelFormat _pixel_format{};
		AVRational _frame_rate{};

		#pragma region 通过 IAudioStreamInfoCollection 继承
	public:
		AVRational TimeBase() const override;
		void SetTimeBase(AVRational value) override;
		AVSampleFormat sample_format() const override;
		void set_sample_format(AVSampleFormat value) override;
		int SampleRate() const override;
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
		AVRational FrameRate() const override;
		void SetFrameRate(AVRational value) override;
		#pragma endregion
	};
}
