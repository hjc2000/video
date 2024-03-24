#pragma once
#include<AVCalculate.h>
#include<Exception.h>
#include<Wrapper.h>
#include<include_ffmpeg.h>

namespace video
{
	class AVFrameWrapper;
	class AVCodecContextWrapper;

	class AVPacketWrapper : public Wrapper<AVPacket>
	{
	public:
		AVPacketWrapper();
		AVPacketWrapper(AVPacketWrapper const &another);
		~AVPacketWrapper();

	public:
		AVPacketWrapper &operator=(AVPacketWrapper const &another);

	public:
		void ChangeTimeBase(AVRational new_time_base);

		int StreamIndex() const;
		void SetStreamIndex(int value);

		/// <summary>
		///		让本包引用另一个包的缓冲区，并且复制其它参数。
		///		在引用另一个包之前会先调用 unref 方法。
		/// </summary>
		/// <param name="other"></param>
		void ref(const AVPacketWrapper &other);

		/// <summary>
		///		解除此包对缓冲区的引用
		/// </summary>
		void unref();

		int64_t Duration() const
		{
			return _wrapped_obj->duration;
		}
		void SetDuration(int64_t value)
		{
			_wrapped_obj->duration = value;
		}

		int64_t pts() const
		{
			return _wrapped_obj->pts;
		}
		void set_pts(int64_t value)
		{
			_wrapped_obj->pts = value;
		}

		int64_t dts() const
		{
			return _wrapped_obj->dts;
		}
		void set_dts(int64_t value)
		{
			_wrapped_obj->dts = value;
		}

		AVRational time_base() const
		{
			return _wrapped_obj->time_base;
		}
		void set_time_base(AVRational value)
		{
			_wrapped_obj->time_base = value;
		}
	};
}