#include <Wraper.h>
#include <AVUtil.h>
extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
}

namespace FFmpeg
{
	class AVStream : public Wraper<::AVStream>
	{
	public:
		AVStream() {}
		AVStream(::AVStream *pAVStream)
		{
			m_pWrapedObj = pAVStream;
		}
		AVStream(::AVStream &ref_AVStream)
		{
			m_pWrapedObj = &ref_AVStream;
		}
	};
}