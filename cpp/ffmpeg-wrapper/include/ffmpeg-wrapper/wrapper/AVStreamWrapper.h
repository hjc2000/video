#pragma once
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include<jccpp/Wrapper.h>

namespace video
{
	class AVCodecWrapper;
	class AVCodecContextWrapper;

	class AVStreamWrapper :
		public Wrapper<AVStream>,
		public IAudioStreamInfoCollection,
		public IVideoStreamInfoCollection
	{
	public:
		AVStreamWrapper();
		AVStreamWrapper(AVStream *p);
		AVStreamWrapper(AVStreamWrapper const &other);

		AVStreamWrapper &operator=(AVStreamWrapper const &other);

	public:
		/// <summary>
		///		获取本流的编解码器参数
		/// </summary>
		/// <returns></returns>
		AVCodecParameters &CodecParams() const;
		/// <summary>
		///		设置本流的编解码器参数。
		/// </summary>
		/// <param name="params"></param>
		void SetCodecParams(AVCodecParameters const &params);

		/// <summary>
		///		通过码器上下文设置此流信息。除了会用码器上下文来设置本流的 AVCodecParameters 外，还会
		///		额外设置时间基和帧率。如果本流是音频流，帧率是没用的，但是设置了也无伤大雅。
		/// </summary>
		/// <param name="codec_ctx">此码器上下文的信息将被复制到此流中</param>
		/// <returns>设置成功则返回值大于等于0，失败则返回小于 0 的错误代码</returns>
		int SetCodecParams(shared_ptr<AVCodecContextWrapper> codec_ctx);

		/// <summary>
		///		获取此流对应的码器
		/// </summary>
		/// <returns></returns>
		AVCodec const *Codec();

		int64_t Bitrate();
		AVMediaType MediaType();

		/// <summary>
		///		获取本流在格式中的索引值
		/// </summary>
		/// <returns></returns>
		int Index();

		#pragma region IAudioStreamInfoCollection,IVideoStreamInfoCollection
		AVSampleFormat sample_format() override;
		void set_sample_format(AVSampleFormat value) override;
		AVChannelLayout ch_layout() override;
		void set_ch_layout(AVChannelLayout value) override;
		int sample_rate() override;
		void set_sample_rate(int value) override;

		int width() override;
		void set_width(int value) override;
		int height() override;
		void set_height(int value) override;
		AVPixelFormat pixel_format() override;
		void set_pixel_format(AVPixelFormat value) override;
		AVRational time_base() override;
		void set_time_base(AVRational value) override;
		AVRational frame_rate() override;
		void set_frame_rate(AVRational value) override;
		#pragma endregion
	};
}
