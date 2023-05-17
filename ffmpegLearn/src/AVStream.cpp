#include <AVStream.h>
#include <AVPacket.h>
#include <AVFormatContext.h>

/**
 * @brief Construct a new FFmpeg::AVStream::AVStream object
 *
 * @param stream
 * @param format
 */
FFmpeg::AVStream::AVStream(::AVStream *stream, AVFormatContext *format)
{
	_baseObj = stream;
	_affFormat = format;
}

/**
 * @brief 将流的参数复制到指定的流
 *
 * @param dstStream
 */
void FFmpeg::AVStream::CopyParamTo(shared_ptr<FFmpeg::AVStream> dstStream)
{
	int result = avcodec_parameters_copy(dstStream->get_Base()->codecpar, _baseObj->codecpar);
	if (result < 0)
		throw result;
}

/**
 * @brief 获取流的媒体类型
 *
 * @return ::AVMediaType
 */
::AVMediaType FFmpeg::AVStream::get_MediaType(void)
{
	return _baseObj->codecpar->codec_type;
}

int FFmpeg::AVStream::get_StreamIndex(void)
{
	return _baseObj->index;
}

/**
 * @brief 获取目标索引值。
 * 在将此流复制到其它封装中时，此值决定了此流在其它封装中的索引值。
 *
 * @return int
 */
int FFmpeg::AVStream::get_DstStreamIndex(void)
{
	return _dstStreamIndex;
}

/**
 * @brief 设置目标索引值
 * 在将此流复制到其它封装中时，此值决定了此流在其它封装中的索引值。
 *
 * @param dstIndex
 */
void FFmpeg::AVStream::set_DstStreamIndex(int dstIndex)
{
	_dstStreamIndex = dstIndex;
}

int64_t FFmpeg::AVStream::get_StartDts(void)
{
	return _startDts;
}

void FFmpeg::AVStream::set_StartDts(int64_t value)
{
	_startDts = value;
}

int64_t FFmpeg::AVStream::get_StartPts(void)
{
	return _startPts;
}

void FFmpeg::AVStream::set_StartPts(int64_t value)
{
	_startPts = value;
}

/**
 * @brief 获取此流所附属于的封装
 *
 * @return FFmpeg::AVFormatContext* const
 */
FFmpeg::AVFormatContext *const FFmpeg::AVStream::get_AffFormat(void)
{
	return _affFormat;
}
