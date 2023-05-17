#include <AVFormatContext.h>
#include <AVPacket.h>
#include <AVStream.h>
#include <iostream>

/**
 * @brief Construct a new FFmpeg::AVFormatContext::AVFormatContext object
 *
 * @param url
 * @param mode
 */
FFmpeg::AVFormatContext::AVFormatContext(char const *url, FormatMode mode)
{
	// 尝试打开文件
	switch (mode)
	{
	case FormatMode::Open:
	{
		int result = avformat_open_input(&_baseObj, url, NULL, NULL);
		if (result < 0)
			throw result;
		InitStreamVector();
		break;
	}
	case FormatMode::Create:
	{
		CreateOutputFormat(url);
		break;
	}
	}
}

/**
 * @brief Destroy the FFmpeg::AVFormatContext::AVFormatContext object
 *
 */
FFmpeg::AVFormatContext::~AVFormatContext()
{
	if (_baseObj)
	{
		/*此函数内部会调用 avformat_free_context 和 avio_close 函数*/
		avformat_close_input(&_baseObj);
	}
}

/**
 * @brief 创建一个空的 AVFormatContext 用来进行输出。相当于创建新文件。
 * 创建完成后还会为这个 AVFormatContext 打开 IO
 *
 * @param url
 */
void FFmpeg::AVFormatContext::CreateOutputFormat(char const *url)
{
	// 为上下文分配资源
	int result = avformat_alloc_output_context2(&_baseObj, NULL, NULL, url);
	if (result < 0)
		throw result;
	// 为这个新创建的 AVFormatContext 打开 IO
	result = avio_open2(&_baseObj->pb, url, AVIO_FLAG_READ_WRITE, NULL, NULL);
	if (result < 0)
		throw result;
}

void FFmpeg::AVFormatContext::InitStreamVector(void)
{
	int numOfStream = get_NumOfStream();
	for (int i = 0; i < numOfStream; i++)
	{
		shared_ptr<FFmpeg::AVStream> stream = make_shared<FFmpeg::AVStream>(_baseObj->streams[i], this);
		_streams.push_back(stream);
	}
}

shared_ptr<FFmpeg::AVStream> FFmpeg::AVFormatContext::GetBestStream(AVMediaType type)
{
	int streamIndex = av_find_best_stream(_baseObj, type, -1, -1, NULL, 0);
	if (streamIndex < 0)
	{
		cout << "[av_find_best_stream] 找不到指定的流" << endl;
		throw streamIndex;
	}
	/*共享指针销毁后，引用计数会减1。函数返回时，先将返回值在堆栈中
	复制一份,此时引用计数加1，然后这个函数中的共享指针对象销毁，引用计数
	减1，所以，调用此函数，接收到的智能指针的引用计数是1*/
	return GetStream(streamIndex);
}

/**
 * @brief 获取指定索引号的流。如果索引号溢出，会抛出异常
 *
 * @param streamIndex
 * @return shared_ptr<FFmpeg::AVStream>
 */
shared_ptr<FFmpeg::AVStream> FFmpeg::AVFormatContext::GetStream(int streamIndex)
{
	if (streamIndex < 0 || streamIndex >= get_NumOfStream())
	{
		cerr << "流的索引号超出范围" << endl;
		throw -1;
	}
	return _streams[streamIndex];
}

/**
 * @brief 创建指定数量的流
 *
 * @param numOfStream 要创建的流的个数
 */
void FFmpeg::AVFormatContext::CreateNewStream(int numOfStream)
{
	if (_streams.size())
	{
		cout << "不要重复调用 CreateNewStream" << endl;
		throw -1;
	}
	for (int i = 0; i < numOfStream; i++)
	{
		::AVStream *stream = avformat_new_stream(_baseObj, NULL);
		if (!stream)
		{
			cout << "创建新流失败" << endl;
			throw -1;
		}
	}
	InitStreamVector();
}

/**
 * @brief 向封装写入头部信息。需要在设置好流的参数后再调用此函数，否则会
 * 导致输出的文件大小为 0kB。
 *
 */
void FFmpeg::AVFormatContext::WriteHeader(void)
{
	int result = avformat_write_header(_baseObj, NULL);
	if (result < 0)
		throw result;
}

/**
 * @brief 向封装写入尾部信息
 *
 */
void FFmpeg::AVFormatContext::WriteTrailer(void)
{
	int result = av_write_trailer(_baseObj);
	if (result < 0)
		throw result;
}

/**
 * @brief 向本封装中写入包
 *
 * @param pkt
 */
void FFmpeg::AVFormatContext::Interleaved_Write_Frame(shared_ptr<FFmpeg::AVPacket> pkt)
{
	pkt->get_Base()->pos = -1;
	int result = av_interleaved_write_frame(_baseObj, pkt->get_Base());
	if (result < 0)
		throw result;
}

/**
 * @brief 读取一个包
 *
 * @return shared_ptr<FFmpeg::AVPacket> 如果失败或读到尽头会返回
 * 空指针
 */
shared_ptr<FFmpeg::AVPacket> FFmpeg::AVFormatContext::ReadFrame()
{
	shared_ptr<FFmpeg::AVPacket> pkt = make_shared<FFmpeg::AVPacket>(this);
	int result = av_read_frame(_baseObj, pkt->get_Base());
	if (result < 0)
	{
		return nullptr;
	}
	else
	{
		pkt->AdjustStartPtsAndDts();
		return pkt;
	}
}

void FFmpeg::AVFormatContext::SeekFrame(double dstTime)
{
	int result = av_seek_frame(_baseObj, -1, dstTime * AV_TIME_BASE, AVSEEK_FLAG_BACKWARD);
	if (result < 0)
		throw result;
}

/**
 * @brief 将本封装的所有流复制到指定的封装中
 *
 * @param dstFormat 要复制到的封装
 */
void FFmpeg::AVFormatContext::CopyAllStreamTo(AVFormatContext &dstFormat)
{
	// 复制头部信息
	for (int i = 0; i < get_NumOfStream(); i++)
	{
		GetStream(i)->CopyParamTo(dstFormat.GetStream(i));
	}
	dstFormat.WriteHeader();

	// 复制包
	shared_ptr<FFmpeg::AVPacket> pkt = nullptr;
	while ((pkt = ReadFrame()))
	{
		int dstStreamIndex = pkt->get_AffStreamIndex();
		shared_ptr<FFmpeg::AVStream> dstStream = dstFormat.GetStream(dstStreamIndex);
		pkt->RescaleTsFor(dstStream);
		pkt->set_AffStreamIndex(dstStreamIndex);
		dstFormat.Interleaved_Write_Frame(pkt);
	}

	// 写尾部
	dstFormat.WriteTrailer();
}

/**
 * @brief 将本封装的流复制到指定封装中。使用 map 进行映射
 *
 * @param dstFormat 要复制到的封装
 * @param streamMap map 数组的头指针。数组元素的个数要与本封装中流的
 * 个数一样，否则会出现超出数组边界的访问
 * @param endtime 结束时间，单位 s
 */
void FFmpeg::AVFormatContext::CopyStreamTo(AVFormatContext &dstFormat, double endtime)
{
	// 复制头部信息
	for (std::shared_ptr<FFmpeg::AVStream> stream : get_StreamList())
	{
		if (stream->get_DstStreamIndex() < 0)
		{
			continue;
		}
		else
		{
			stream->CopyParamTo(dstFormat.GetStream(stream->get_DstStreamIndex()));
		}
	}
	dstFormat.WriteHeader();

	// 复制包
	shared_ptr<FFmpeg::AVPacket> pkt = nullptr;
	while ((pkt = ReadFrame()))
	{
		int sourceStreamIndex = pkt->get_AffStreamIndex();
		std::shared_ptr<FFmpeg::AVStream> affStream = GetStream(sourceStreamIndex);
		if (av_q2d(affStream->get_Base()->time_base) * pkt->get_Pts() > endtime)
		{
			cout << "提前结束" << endl;
			break;
		}
		int dstStreamIndex = affStream->get_DstStreamIndex();
		if (dstStreamIndex < 0)
		{
			continue;
		}
		else
		{
			shared_ptr<FFmpeg::AVStream> dstStream = dstFormat.GetStream(dstStreamIndex);
			pkt->RescaleTsFor(dstStream);
			pkt->set_AffStreamIndex(dstStreamIndex);
			dstFormat.Interleaved_Write_Frame(pkt);
		}
	}
}

/**
 * @brief 获取流的个数
 *
 * @return int 流的个数
 */
int FFmpeg::AVFormatContext::get_NumOfStream(void)
{
	return _baseObj->nb_streams;
}

vector<shared_ptr<FFmpeg::AVStream>> &FFmpeg::AVFormatContext::get_StreamList(void)
{
	return _streams;
}
