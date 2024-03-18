#pragma once
#include<AVDictionaryWrapper.h>
#include<AVFilterContextWrapper.h>
#include<IFrameConsumer.h>
#include<IFrameSource.h>
#include<VideoStreamInfoCollection.h>

namespace video
{
	class VideoFilterGraph :
		public Wrapper<AVFilterGraph>,
		public IFrameConsumer,
		public IFrameSource
	{
	public:
		VideoFilterGraph(IVideoStreamInfoCollection &infos) :
			_input_video_stream_infos(infos)
		{
			_wrapped_obj = avfilter_graph_alloc();
			if (!_wrapped_obj)
			{
				throw jc::Exception();
			}

			init_buffer_filter();
			init_buffer_sink_filter();
		}

	private:
		VideoStreamInfoCollection _input_video_stream_infos;

		AVFilterContextWrapper _buffer_filter;
		void init_buffer_filter()
		{
			const AVFilter *buffersrc = avfilter_get_by_name("buffer");
			AVFilterContext *buffersrc_ctx = avfilter_graph_alloc_filter(_wrapped_obj, buffersrc, "src");
			if (!buffersrc_ctx)
			{
				throw jc::Exception();
			}

			av_opt_set_int(buffersrc_ctx, "width", _input_video_stream_infos.width(), AV_OPT_SEARCH_CHILDREN);
			av_opt_set_int(buffersrc_ctx, "height", _input_video_stream_infos.height(), AV_OPT_SEARCH_CHILDREN);
			av_opt_set_pixel_fmt(buffersrc_ctx, "pix_fmt", _input_video_stream_infos.pixel_format(), AV_OPT_SEARCH_CHILDREN);
			av_opt_set_q(buffersrc_ctx, "time_base", _input_video_stream_infos.time_base(), AV_OPT_SEARCH_CHILDREN);
			av_opt_set_q(buffersrc_ctx, "frame_rate", _input_video_stream_infos.frame_rate(), AV_OPT_SEARCH_CHILDREN);
			if (avfilter_init_str(buffersrc_ctx, nullptr) < 0)
			{
				throw jc::Exception();
			}

			_buffer_filter = AVFilterContextWrapper{ buffersrc_ctx };
		}

		AVFilterContextWrapper _buffer_sink_filter;
		void init_buffer_sink_filter()
		{
			const AVFilter *buffersink = avfilter_get_by_name("buffersink");
			AVFilterContext *buffersink_ctx = avfilter_graph_alloc_filter(_wrapped_obj, buffersink, "sink");
			if (!buffersink_ctx)
			{
				throw jc::Exception();
			}

			if (avfilter_init_str(buffersink_ctx, nullptr) < 0)
			{
				throw jc::Exception();
			}

			_buffer_sink_filter = AVFilterContextWrapper{ buffersink_ctx };
		}

	public:
		AVFilterContextWrapper &buffer_filter()
		{
			return _buffer_filter;
		}

		AVFilterContextWrapper &buffer_sink_filter()
		{
			return _buffer_sink_filter;
		}

		/**
		 * @brief 构造一个 fps 滤镜。稍后需要用 link_filters 方法连接。
		 * @param frame_rate
		 * @return
		*/
		AVFilterContextWrapper alloc_fps_filter(AVRational frame_rate)
		{
			const AVFilter *fps_filter = avfilter_get_by_name("fps");
			AVFilterContext *fps_ctx = avfilter_graph_alloc_filter(_wrapped_obj, fps_filter, "fps");
			if (!fps_ctx)
			{
				throw jc::Exception();
			}

			AVDictionaryWrapper options;
			options.SetValueByKey("fps", ToString(frame_rate).c_str());
			if (avfilter_init_dict(fps_ctx, options) < 0)
			{
				throw jc::Exception();
			}

			return AVFilterContextWrapper{ fps_ctx };
		}

		void config_graph()
		{
			if (avfilter_graph_config(_wrapped_obj, nullptr) < 0)
			{
				throw jc::Exception();
			}
		}

		int ReadFrame(AVFrameWrapper &frame) override
		{
			frame.unref();
			return av_buffersink_get_frame(_buffer_sink_filter, frame);
		}

		void SendFrame(AVFrameWrapper *frame) override
		{
			if (!frame)
			{
				// 冲洗模式
				if (av_buffersrc_add_frame_flags(_buffer_filter, nullptr, AV_BUFFERSRC_FLAG_KEEP_REF) < 0)
				{
					throw SendFrameException();
				}

				return;
			}

			// 非冲洗模式
			if (av_buffersrc_add_frame_flags(_buffer_filter, *frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0)
			{
				throw SendFrameException();
			}
		}
	};
}
