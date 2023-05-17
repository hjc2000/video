#include <AVPacket.h>
#include <AVStream.h>
#include <AVFormatContext.h>

/**
 * @brief Construct a new FFmpeg::AVPacket::AVPacket object
 *
 * @param affiliatedStream 此包所属的流
 */
FFmpeg::AVPacket::AVPacket(AVFormatContext *affFormat)
{
	_baseObj = &_pkt;
	_affFormat = affFormat;
}

FFmpeg::AVPacket::~AVPacket()
{
	/*析构函数里面不要使用 av_packet_free，否则会造成输出
	文件大小为0.推测可能是包还没被写入文件中就被释放了*/
	UnRef();
}

void FFmpeg::AVPacket::RescaleTsFor(shared_ptr<FFmpeg::AVStream> dstStream)
{
	av_packet_rescale_ts(_baseObj, _affFormat->GetStream(get_AffStreamIndex())->get_Base()->time_base, dstStream->get_Base()->time_base);
}

void FFmpeg::AVPacket::UnRef(void)
{
	av_packet_unref(&_pkt);
}

int FFmpeg::AVPacket::get_AffStreamIndex()
{
	return _baseObj->stream_index;
}

/**
 * @brief 在将这个包写入别的封装之前需要使用此访问器设置一下流的索引，
 * 这样写到别的封装后，此包就处于指定的流中
 *
 * @param index
 */
void FFmpeg::AVPacket::set_AffStreamIndex(int index)
{
	_baseObj->stream_index = index;
}

int64_t FFmpeg::AVPacket::get_Pts(void)
{
	return _baseObj->pts;
}

void FFmpeg::AVPacket::set_Pts(int64_t pts)
{
	_baseObj->pts = pts;
}

int64_t FFmpeg::AVPacket::get_Dts(void)
{
	return _baseObj->dts;
}

void FFmpeg::AVPacket::set_Dts(int64_t dts)
{
	_baseObj->dts = dts;
}

/**
 * @brief 在调用 av_read_frame 后看看，如果自己是流中的第一个包，则将流的
 * 起始 dts 和 pts 设置为自己的 dts, pts。然后将自己的 pts 和 dts
 * 设置为 0，如果自己不是第一个包则将调整自己的 dts 和 pts
 *
 * 此方法是私有的，只让友元类 AVFormatContext 使用
 */
void FFmpeg::AVPacket::AdjustStartPtsAndDts(void)
{
	int affStreamIndex = get_AffStreamIndex();
	std::shared_ptr<FFmpeg::AVStream> affStream = _affFormat->GetStream(affStreamIndex);

	int startDts = affStream->get_StartDts();
	if (startDts < 0)
	{
		affStream->set_StartDts(_baseObj->dts);
		_baseObj->dts = 0;
	}
	else
	{
		_baseObj->dts = _baseObj->dts - affStream->get_StartDts();
	}

	int startPts = affStream->get_StartPts();
	if (startPts < 0)
	{
		affStream->set_StartPts(_baseObj->pts);
		_baseObj->pts = 0;
	}
	else
	{
		_baseObj->pts = _baseObj->pts - affStream->get_StartPts();
	}

	if (_baseObj->dts > _baseObj->pts)
	{
		_baseObj->pts = _baseObj->dts;
	}
}
