#include "AVFormatContext.h"
#include<AVUtil.h>
#include<AVPacket.h>
#include<AVDictionary.h>
#include<AVStream.h>
#include<AVError.h>

FFmpeg::AVFormatContext::~AVFormatContext()
{
	cout << "AVFormatContext 析构" << endl;
	if (_mode == Mode::output)
	{
		if (!(w->oformat->flags & AVFMT_NOFILE))
		{
			avio_closep(&w->pb);
		}
	}
	// 不管是输入还是输出，尽管调用释放资源的函数，反正不会发生异常
	// ffmpeg 内部有防御措施，不会对 nullptr 执行释放资源的操作
	::avformat_close_input(&w);
	::avformat_free_context(w);
}

shared_ptr<FFmpeg::AVFormatContext> FFmpeg::AVFormatContext::open_input(const char *url, const::AVInputFormat *fmt, ::AVDictionary **options)
{
	shared_ptr<FFmpeg::AVFormatContext> p{new FFmpeg::AVFormatContext()};
	int ret = ::avformat_open_input(&p->w, url, fmt, options);
	if (ret < 0)
	{
		throw Exception("AVFormatContext::open_input", ret);
	}
	return p;
}

shared_ptr<FFmpeg::AVFormatContext> FFmpeg::AVFormatContext::alloc_output_context2(const char *filename)
{
	shared_ptr<FFmpeg::AVFormatContext> p{new FFmpeg::AVFormatContext()};
	int ret = ::avformat_alloc_output_context2(&p->w, nullptr, nullptr, filename);
	if (ret < 0)
	{
		throw Exception("alloc_output_context2", ret);
	}

	// 如果没有打开 IO 则打开 IO
	if (!(p->w->oformat->flags & AVFMT_NOFILE))
	{
		int ret = avio_open(&p->w->pb, filename, AVIO_FLAG_WRITE);
		if (ret < 0)
		{
			throw Exception("AVFormatContext::alloc_output_context2", ret);
		}
	}

	return p;
}

void FFmpeg::AVFormatContext::dump_format(int index, const char *url, int is_output)
{
	::av_dump_format(w, index, url, is_output);
}

void FFmpeg::AVFormatContext::dump_format(FFmpeg::AVStream stream, const char *url, int is_output)
{
	dump_format(stream()->index, url, is_output);
}

void FFmpeg::AVFormatContext::find_stream_info(::AVDictionary **options)
{
	int ret = ::avformat_find_stream_info(w, options);
	if (ret < 0)
	{
		throw Exception("AVFormatContext::find_stream_info", ret);
	}
}

shared_ptr<FFmpeg::AVStream> FFmpeg::AVFormatContext::find_best_stream(AVMediaType type)
{
	int ret = ::av_find_best_stream(w, type, -1, -1, nullptr, 0);
	if (ret < 0)
	{
		throw Exception("AVFormatContext::find_best_stream(AVMediaType = " + FFmpeg::media_type_to_string(type) + ")", ret);
	}
	else
	{
		shared_ptr<FFmpeg::AVStream> p{new FFmpeg::AVStream{ w->streams[ret] }};
		return p;
	}
}

int FFmpeg::AVFormatContext::read_packet(FFmpeg::AVPacket packet)
{
	return ::av_read_frame(w, packet);
}

FFmpeg::AVStream FFmpeg::AVFormatContext::create_new_stream(const::AVCodec *pCodec)
{
	::AVStream *ps = avformat_new_stream(w, pCodec);
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
		ret = ::avformat_write_header(w, nullptr);
	else
		ret = ::avformat_write_header(w, *dic);

	if (ret < 0)
	{
		cout << "write_header 方法异常：" << FFmpeg::error_code_to_str(ret) << endl;
		throw Exception("AVFormatContext::write_header", ret);
	}
}

void FFmpeg::AVFormatContext::interleaved_write_packet(FFmpeg::AVPacket packet)
{
	int ret = ::av_interleaved_write_frame(w, packet);
	if (ret < 0)
	{
		cout << "interleaved_write_packet 异常："
			<< FFmpeg::error_code_to_str(ret) << endl;
		throw Exception("AVFormatContext::receive_frame", ret);
	}
}

void FFmpeg::AVFormatContext::write_trailer()
{
	int ret = ::av_write_trailer(w);
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
	int64_t total_seconds = w->duration / AV_TIME_BASE;
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
	if ((uint32_t)stream_index >= w->nb_streams)
	{
		throw Exception("流索引号超出范围");
	}

	return FFmpeg::AVStream{w->streams[stream_index]};
}
