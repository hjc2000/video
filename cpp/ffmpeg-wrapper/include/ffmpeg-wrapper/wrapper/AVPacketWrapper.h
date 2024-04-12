#pragma once
#include<ffmpeg-wrapper/AVCalculate.h>
#include<jccpp/Exception.h>
#include<jccpp/Wrapper.h>
#include<ffmpeg-wrapper/base_include.h>

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

		int64_t Duration() const;
		void SetDuration(int64_t value);

		int64_t pts() const;
		void set_pts(int64_t value);

		int64_t dts() const;
		void set_dts(int64_t value);

		AVRational TimeBase() const;
		void set_time_base(AVRational value);
	};
}