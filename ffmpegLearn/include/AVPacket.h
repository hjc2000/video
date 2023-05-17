#pragma once
#include <global.h>
using namespace std;

namespace FFmpeg
{
	class AVStream;
	class AVFormatContext;

	class AVPacket : public Wrapper<::AVPacket>
	{
	public:
		AVPacket(AVFormatContext *affFormat);
		~AVPacket();
		void RescaleTsFor(shared_ptr<FFmpeg::AVStream> dstStream);
		void UnRef(void);
		friend class AVFormatContext;

		// 访问器
	public:
		int get_AffStreamIndex();
		int64_t get_Pts(void);
		void set_Pts(int64_t pts);
		int64_t get_Dts(void);
		void set_Dts(int64_t dts);
		void set_AffStreamIndex(int index);

	private:
		::AVPacket _pkt;
		AVFormatContext *_affFormat = nullptr;

		// 私有方法
	private:
		void AdjustStartPtsAndDts(void);
	};

}
