#pragma once
#include <global.h>

namespace FFmpeg
{
	class AVFormatContext;

	class AVStream : public Wrapper<::AVStream>
	{
		// 生命周期
	public:
		AVStream(::AVStream *stream, AVFormatContext *format);

		// 公共方法
	public:
		void CopyParamTo(shared_ptr<FFmpeg::AVStream> dstStream);

		// 属性访问器
	public:
		::AVMediaType get_MediaType(void);
		int get_StreamIndex(void);
		int get_DstStreamIndex(void);
		void set_DstStreamIndex(int dstIndex);
		int64_t get_StartDts(void);
		void set_StartDts(int64_t value);
		int64_t get_StartPts(void);
		void set_StartPts(int64_t value);
		FFmpeg::AVFormatContext *const get_AffFormat(void);

		// 私有字段
	private:
		FFmpeg::AVFormatContext *_affFormat = nullptr;
		int _dstStreamIndex = -1;
		int64_t _startDts = -1;
		int64_t _startPts = -1;
	};

}