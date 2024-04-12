#pragma once
#include<ffmpeg-wrapper/AVChannelLayoutExtension.h>
#include<ffmpeg-wrapper/AVCompare.h>
#include<ffmpeg-wrapper/AVSampleFormatExtention.h>
#include<ffmpeg-wrapper/base_include.h>

namespace video
{
	/// <summary>
	///		继承此接口表示含有：采样格式、采样率、声道布局、时间基 等信息。
	/// </summary>
	class IAudioStreamInfoCollection
	{
	public:
		virtual ~IAudioStreamInfoCollection() {}

	public:
		IAudioStreamInfoCollection &operator=(IAudioStreamInfoCollection const &value)
		{
			*this = (IAudioStreamInfoCollection &)value;
			return *this;
		}

		IAudioStreamInfoCollection &operator=(IAudioStreamInfoCollection &value)
		{
			SetTimeBase(value.TimeBase());
			set_sample_format(value.sample_format());
			SetSampleRate(value.SampleRate());
			SetChannelLayout(value.ChannelLayout());
			return *this;
		}

	public:
		virtual AVRational TimeBase() const = 0;
		virtual void SetTimeBase(AVRational value) = 0;

		virtual AVSampleFormat sample_format() const = 0;
		virtual void set_sample_format(AVSampleFormat value) = 0;

		virtual int SampleRate() const = 0;
		virtual void SetSampleRate(int value) = 0;

		virtual AVChannelLayout ChannelLayout() = 0;
		virtual void SetChannelLayout(AVChannelLayout value) = 0;

	public: // 接口扩展
		/// <summary>
		///		获取两个采样点之间的时间间隔。其实就是采样率的倒数。
		/// </summary>
		/// <returns></returns>
		double sample_interval()
		{
			return 1.0 / SampleRate();
		}

		/// <summary>
		///		两个采样点之间的时间间隔，单位毫秒。
		///		
		///		本属性内部是
		///		1.0 * 1000 / SampleRate()
		///		即先 1.0 乘 1000 再除以 SampleRate()，这比先除以 SampleRate()
		///		再乘 1000 的精度要高。
		/// </summary>
		/// <returns></returns>
		double sample_interval_in_milliseconds()
		{
			return 1.0 * 1000 / SampleRate();
		}

		/// <summary>
		///		将 sample_format 属性转化为字符串。
		/// </summary>
		/// <returns>返回 sample_format 的名称。如果是未知类型，返回空字符串</returns>
		string sample_format_string()
		{
			const char *name = av_get_sample_fmt_name(sample_format());
			return name ? string(name) : "";
		}

		/// <summary>
		///		获取声道布局的描述字符串
		/// </summary>
		/// <returns></returns>
		string channel_layout_description()
		{
			return AVChannelLayoutExtension::channel_layout_description(ChannelLayout());
		}

		/// <summary>
		///		判断本音频帧是不是平面类型。当然，本帧首先得是音频帧。
		/// </summary>
		/// <returns></returns>
		bool IsPlanar()
		{
			return av_sample_fmt_is_planar(sample_format());
		}

		/// <summary>
		///		获取每个采样点的字节数
		/// </summary>
		/// <returns>每个采样点的字节数</returns>
		int BytesPerSample()
		{
			return av_get_bytes_per_sample(sample_format());
		}
	};
}

/**
 * @brief 比较两个 IAudioStreamInfoCollection 是否相等。
 * @param left
 * @param right
 * @return
*/
inline bool operator==(video::IAudioStreamInfoCollection &left, video::IAudioStreamInfoCollection &right)
{
	return left.TimeBase() == right.TimeBase() &&
		left.sample_format() == right.sample_format() &&
		left.SampleRate() == right.SampleRate() &&
		left.ChannelLayout() == right.ChannelLayout();
}
