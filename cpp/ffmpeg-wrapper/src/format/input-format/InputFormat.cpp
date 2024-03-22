#include"InputFormat.h"
#include<AVDictionaryWrapper.h>
#include<AVPacketWrapper.h>
#include<AVStreamWrapper.h>
#include<AVToString.h>
#include<ErrorCode.h>
#include<include_ffmpeg.h>

using namespace video;

video::InputFormat::InputFormat(std::string url)
{
	_url = url;
	_wrapped_obj = avformat_alloc_context();
	int ret = ::avformat_open_input(&_wrapped_obj, url.c_str(), nullptr, nullptr);
	if (ret < 0)
	{
		cout << CODE_POS_STR << "打开输入格式失败" << endl;
		throw jc::Exception();
	}

	FindStreamInfo();
}

video::InputFormat::InputFormat(std::string url, AVInputFormat const *fmt, AVDictionary **options)
{
	_url = url;
	_wrapped_obj = avformat_alloc_context();
	int ret = ::avformat_open_input(&_wrapped_obj, url.c_str(), fmt, options);
	if (ret < 0)
	{
		cout << CODE_POS_STR << "打开输入格式失败" << endl;
		throw jc::Exception();
	}

	FindStreamInfo();
}

video::InputFormat::InputFormat(shared_ptr<AVIOContextWrapper> io_context)
{
	_io_context = io_context;
	_wrapped_obj = avformat_alloc_context();
	_wrapped_obj->pb = *_io_context;
	_wrapped_obj->flags |= AVFMT_FLAG_CUSTOM_IO;
	int ret = ::avformat_open_input(&_wrapped_obj, nullptr, nullptr, nullptr);
	if (ret < 0)
	{
		cout << CODE_POS_STR << "打开输入格式失败" << endl;
		throw jc::Exception();
	}

	FindStreamInfo();
}

video::InputFormat::InputFormat(shared_ptr<Stream> input_stream) :
	InputFormat(shared_ptr<AVIOContextWrapper>{ new AVIOContextWrapper{ false,input_stream } })
{

}

InputFormat::~InputFormat()
{
	::avformat_close_input(&_wrapped_obj);
	::avformat_free_context(_wrapped_obj);
	_wrapped_obj = nullptr;
}

void InputFormat::DumpFormat()
{
	cout << endl;
	cout << "------------------------------------------------------------" << endl;
	cout << "▼ 格式信息" << endl;
	cout << "------------------------------------------------------------" << endl;
	av_dump_format(_wrapped_obj, 0, _url.c_str(), false);
	cout << "------------------------------------------------------------" << endl;
	cout << endl;
}

void InputFormat::FindStreamInfo(::AVDictionary **options)
{
	int ret = ::avformat_find_stream_info(_wrapped_obj, options);
	if (ret < 0)
	{
		cout << CODE_POS_STR << "查找流信息失败" << endl;
		throw jc::Exception();
	}
}

AVStreamWrapper InputFormat::FindBestStream(AVMediaType type)
{
	int ret = ::av_find_best_stream(
		_wrapped_obj,
		type,
		-1,
		-1,
		nullptr,
		0
	);

	if (ret < 0)
	{
		cout << CODE_POS_STR << "找不到最好的 " << ToString(type) << " 流" << endl;
		return nullptr;
	}

	return _wrapped_obj->streams[ret];
}

int InputFormat::ReadPacket(AVPacketWrapper &packet)
{
	packet.unref();
	int ret = ::av_read_frame(_wrapped_obj, packet);
	if (!ret)
	{
		packet->time_base = _wrapped_obj->streams[packet->stream_index]->time_base;
	}

	return ret;
}

std::string InputFormat::get_duration_as_formatted_time_string()
{
	std::stringstream sstream;

	// 获取视频总的秒数
	int64_t total_seconds = _wrapped_obj->duration / AV_TIME_BASE;

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

int InputFormat::StreamCount()
{
	return _wrapped_obj->nb_streams;
}

AVStreamWrapper InputFormat::GetStream(int stream_index)
{
	// 强制转换为无符号类型就不用判断 stream_index >= 0 了
	if ((uint32_t)stream_index >= _wrapped_obj->nb_streams)
	{
		cout << CODE_POS_STR << "流索引号超出范围" << endl;
		throw jc::Exception();
	}

	return AVStreamWrapper{ _wrapped_obj->streams[stream_index] };
}
