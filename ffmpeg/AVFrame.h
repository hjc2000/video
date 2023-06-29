#pragma once
#include<Wraper.h>
#include<include_ffmpeg.h>

namespace FFmpeg
{
	class AVFrame :public Wraper<::AVFrame>
	{
	public:
		AVFrame() :Wraper() {}
		AVFrame(::AVFrame *p)
		{
			_pWrapedObj = p;
		}

		~AVFrame()
		{
			Dispose();
		}
		
		void Dispose()
		{
			if (should_dispose())
			{
				cout << "AVFrame 析构" << endl;
				av_frame_free(&_pWrapedObj);
			}
		}

		#pragma region 工厂函数
	public:
		static FFmpeg::AVFrame create()
		{
			FFmpeg::AVFrame frame;
			frame._pWrapedObj = av_frame_alloc();
			if (!frame._pWrapedObj)
				throw "av_frame_alloc 失败";
			return frame;
		}
		#pragma endregion

		#pragma region 运算符重载
	public:
		/// <summary>
		/// 重载赋值运算符
		/// </summary>
		/// <param name="ref"></param>
		void operator=(const FFmpeg::AVFrame &ref)
		{
			// 防止自赋值
			if (this == &ref) return;
			cout << "AVFrame 赋值运算符" << endl;
			// 如果本对象已经分配 _pWrapedObj 资源了就要先释放
			if (_pWrapedObj)
				Dispose();
			// 将对方的资源拿过来
			_pWrapedObj = ref._pWrapedObj;
			// 递增引用计数，只要是复制 _pWrapedObj，必须同时复制 _refCount
			_refCount = ref._refCount;
		}
		#pragma endregion

	public:
		void av_frame_get_buffer(int align)
		{
			int ret = ::av_frame_get_buffer(_pWrapedObj, align);
			if (ret < 0)
				throw ret;
		}

		void av_frame_make_writable()
		{
			int ret = ::av_frame_make_writable(_pWrapedObj);
			if (ret < 0)
				throw ret;
		}
	};
}
