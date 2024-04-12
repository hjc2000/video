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
		AVCodec const *Codec() const;

		int64_t Bitrate() const;
		AVMediaType MediaType() const;

		/// <summary>
		///		获取本流在格式中的索引值
		/// </summary>
		/// <returns></returns>
		int Index() const;

		#pragma region IAudioStreamInfoCollection,IVideoStreamInfoCollection
		AVSampleFormat SampleFormat() const override;
		void SetSampleFormat(AVSampleFormat value) override;
		AVChannelLayout ChannelLayout() const override;
		void SetChannelLayout(AVChannelLayout value) override;
		int SampleRate() const override;
		void SetSampleRate(int value) override;

		int Width() const override;
		void SetWidth(int value) override;
		int Height() const override;
		void SetHeight(int value) override;
		AVPixelFormat PixelFormat() const override;
		void SetPixelFormat(AVPixelFormat value) override;
		AVRational TimeBase() const override;
		void SetTimeBase(AVRational value) override;
		AVRational FrameRate() const override;
		void SetFrameRate(AVRational value) override;
		#pragma endregion
	};
}
