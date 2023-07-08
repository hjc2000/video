#include "AVFormatContext.h"
#include<AVUtil.h>
#include<AVPacket.h>
#include<AVDictionary.h>
#include<AVStream.h>
#include<AVError.h>

FFmpeg::AVFormatContext::~AVFormatContext()
{
	if (should_dispose())
	{
		cout << "AVFormatContext 释放" << endl;
		if (_mode == Mode::output)
		{
			if (!(_pWrapedObj->oformat->flags & AVFMT_NOFILE))
				avio_closep(&_pWrapedObj->pb);
		}
		// 不管是输入还是输出，尽管调用释放资源的函数，反正不会发生异常
		// ffmpeg 内部有防御措施，不会对 nullptr 执行释放资源的操作
		::avformat_close_input(&_pWrapedObj);
		::avformat_free_context(_pWrapedObj);
	}
}

void FFmpeg::AVFormatContext::open_input(const char *url, const::AVInputFormat *fmt, ::AVDictionary **options)
{
	if (_mode != Mode::unknow)
		throw Exception("非法操作，不要重复初始化");
	_mode = Mode::input;
	int ret = ::avformat_open_input(&_pWrapedObj, url, fmt, options);
	if (ret < 0)
		throw Exception("AVFormatContext::open_input", ret);
}

void FFmpeg::AVFormatContext::alloc_output_context2(const char *filename)
{
	if (_mode != Mode::unknow)
		throw Exception("非法操作，不要重复初始化");
	_mode = Mode::output;
	int ret = ::avformat_alloc_output_context2(&_pWrapedObj, nullptr, nullptr, filename);
	if (ret < 0)
		throw Exception("alloc_output_context2", ret);
	// 如果没有打开 IO 则打开 IO
	if (!(_pWrapedObj->oformat->flags & AVFMT_NOFILE))
	{
		int ret = avio_open(&_pWrapedObj->pb, filename, AVIO_FLAG_WRITE);
		if (ret < 0)
			throw Exception("AVFormatContext::alloc_output_context2", ret);
	}
}

void FFmpeg::AVFormatContext::dump_format(int index, const char *url, int is_output)
{
	::av_dump_format(_pWrapedObj, index, url, is_output);
}

void FFmpeg::AVFormatContext::dump_format(FFmpeg::AVStream stream, const char *url, int is_output)
{
	dump_format(stream()->index, url, is_output);
}

void FFmpeg::AVFormatContext::find_stream_info(::AVDictionary **options)
{
	int ret = ::avformat_find_stream_info(_pWrapedObj, options);
	if (ret < 0)
	{
		throw Exception("AVFormatContext::find_stream_info", ret);
	}
}

FFmpeg::AVStream FFmpeg::AVFormatContext::find_best_stream(AVMediaType type)
{
	int ret = ::av_find_best_stream(_pWrapedObj, type, -1, -1, nullptr, 0);
	if (ret < 0)
		throw Exception("AVFormatContext::find_best_stream(AVMediaType = " + FFmpeg::media_type_to_string(type) + ")", ret);
	else
		return _pWrapedObj->streams[ret];
}

int FFmpeg::AVFormatContext::read_packet(FFmpeg::AVPacket packet)
{
	return ::av_read_frame(_pWrapedObj, packet);
}

FFmpeg::AVStream FFmpeg::AVFormatContext::create_new_stream(const::AVCodec *pCodec)
{
	::AVStream *ps = avformat_new_stream(_pWrapedObj, pCodec);
	if (ps == nullptr)
	{
		cout << "create_new_stream 方法异常" << endl;
		throw Exception("创建流失败");
	}
	else
	{
		return ps;
	}
}

void FFmpeg::AVFormatContext::write_header(FFmpeg::AVDictionary *dic)
{
	int ret;
	if (dic == nullptr)
		ret = ::avformat_write_header(_pWrapedObj, nullptr);
	else
		ret = ::avformat_write_header(_pWrapedObj, *dic);

	if (ret < 0)
	{
		cout << "write_header 方法异常：" << FFmpeg::error_code_to_str(ret) << endl;
		throw Exception("AVFormatContext::write_header", ret);
	}
}

void FFmpeg::AVFormatContext::interleaved_write_packet(FFmpeg::AVPacket packet)
{
	int ret = ::av_interleaved_write_frame(_pWrapedObj, packet);
	if (ret < 0)
	{
		cout << "interleaved_write_packet 异常："
			<< FFmpeg::error_code_to_str(ret) << endl;
		throw Exception("AVFormatContext::receive_frame", ret);
	}
}

void FFmpeg::AVFormatContext::write_trailer()
{
	int ret = ::av_write_trailer(_pWrapedObj);
	if (ret < 0)
	{
		cout << "write_trailer 异常：" <<
			FFmpeg::error_code_to_str(ret) << endl;
		throw Exception("AVFormatContext::receive_frame", ret);
	}
}

std::string FFmpeg::AVFormatContext::get_duration_as_formatted_time_string()
{
	std::stringstream sstream;
	// 获取视频总的秒数
	int64_t total_seconds = _pWrapedObj->duration / AV_TIME_BASE;
	// 取出秒
	int second = total_seconds % 60;
	// 扣除秒位后总的分钟数
	uint64_t total_minutes = total_seconds / 60;
	// 取出分钟
	int minute = total_minutes % 60;
	// 扣除分钟后总的小时数
	uint64_t total_hours = total_minutes / 60;
	sstream << total_hours << "时" << minute << "分" << second << "秒" << std::endl;
	std::string re_value;
	sstream >> re_value;
	return re_value;
}

FFmpeg::AVStream FFmpeg::AVFormatContext::get_stream(int stream_index)
{
	// 强制转换为无符号类型就不用判断 stream_index >= 0 了
	if ((uint32_t)stream_index >= _pWrapedObj->nb_streams)
	{
		throw Exception("流索引号超出范围");
	}

	return FFmpeg::AVStream{_pWrapedObj->streams[stream_index]};
}
