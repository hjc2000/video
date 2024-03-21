#pragma once
#include<Wrapper.h>
#include<include_ffmpeg.h>

namespace video
{
	class AVFilterContextWrapper :public Wrapper<AVFilterContext>
	{
		#pragma region 生命周期
	public:
		AVFilterContextWrapper() {}

		AVFilterContextWrapper(AVFilterContext *filter_ctx)
		{
			_wrapped_obj = filter_ctx;
		}

		AVFilterContextWrapper(AVFilterContextWrapper const &other)
		{
			_wrapped_obj = other._wrapped_obj;
		}

		AVFilterContextWrapper &operator=(AVFilterContextWrapper const &other)
		{
			_wrapped_obj = other._wrapped_obj;
			return *this;
		}
		#pragma endregion

	public:
		/**
		 * @brief 将下一个过滤器的输入端连接到本过滤器输出端。本过滤器的输出会作为下一个过滤器的输入。
		 * @param next_filter
		*/
		void link(AVFilterContextWrapper &next_filter)
		{
			if (avfilter_link(_wrapped_obj, 0, next_filter, 0))
			{
				throw jc::Exception();
			}
		}

		/**
		 * @brief 将 next_filter 的输入端连接到本对象的输出端。本对象的输出会作为 next_filter 的输入。
		 *
		 * @param next_filter 要连接到本对象输出端的过滤器。
		 *
		 * @return 返回 next_filter，所以 next_filter 右侧还可以继续用 << 运算符，跟着一个要连接到 next_filter
		 * 输出端的过滤器。
		*/
		AVFilterContextWrapper &operator<<(AVFilterContextWrapper &next_filter)
		{
			link(next_filter);
			return next_filter;
		}
	};
}