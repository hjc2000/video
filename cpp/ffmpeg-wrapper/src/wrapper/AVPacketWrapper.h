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
		AVPacketWrapper()
		{
			_wrapped_obj = av_packet_alloc();
			if (_wrapped_obj == nullptr)
			{
				cout << CODE_POS_STR << "构造 AVPacket 失败" << endl;
				throw jc::Exception();
			}
		}

		AVPacketWrapper(AVPacketWrapper const &other) :AVPacketWrapper()
		{
			ref(other);
		}

		void operator=(AVPacketWrapper const &other)
		{
			ref(other);
		}

		~AVPacketWrapper()
		{
			av_packet_free(&_wrapped_obj);
		}

	public:
		void ChangeTimeBase(AVRational new_time_base)
		{
			AVRational old_time_base = _wrapped_obj->time_base;
			_wrapped_obj->time_base = new_time_base;
			_wrapped_obj->pts = ConvertTimeStamp(_wrapped_obj->pts, old_time_base, new_time_base);
			_wrapped_obj->dts = ConvertTimeStamp(_wrapped_obj->dts, old_time_base, new_time_base);
			_wrapped_obj->duration = ConvertTimeStamp(_wrapped_obj->duration, old_time_base, new_time_base);
		}

		int stream_index() const
		{
			return _wrapped_obj->stream_index;
		}
		void set_stream_index(int value)
		{
			_wrapped_obj->stream_index = value;
		}

		/**
		 * @brief 让本包引用另一个包的缓冲区，并且复制其它参数。
		 * - 在引用另一个包之前会先调用 unref 方法。
		 *
		 * @param other
		*/
		void ref(const AVPacketWrapper &other)
		{
			unref();
			int ret = av_packet_ref(_wrapped_obj, other._wrapped_obj);
			if (ret < 0)
			{
				cout << CODE_POS_STR << "引用 AVPacket 失败" << endl;
				throw jc::Exception();
			}
		}

		/// <summary>
		///		解除此包对缓冲区的引用
		/// </summary>
		void unref()
		{
			av_packet_unref(_wrapped_obj);
		}

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