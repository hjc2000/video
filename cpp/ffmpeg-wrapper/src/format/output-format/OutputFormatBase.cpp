#include "OutputFormatBase.h"
#include<ErrorCode.h>

using namespace video;

void video::OutputFormatBase::SendPacket(AVPacketWrapper *packet)
{
	std::lock_guard l(_not_private_methods_lock);
	if (!packet)
	{
		_flush_times++;
		if (_flush_times == _wrapped_obj->nb_streams)
		{
			cout << CODE_POS_STR << "所有流都被冲洗了。" << endl;
			WriteTrailer();
			if (_on_all_stream_flushed_async)
			{
				std::thread([&]()
				{
					_on_all_stream_flushed_async();
				}).detach();
			}
		}

		return;
	}

	int ret = av_interleaved_write_frame(_wrapped_obj, *packet);
	if (ret < 0)
	{
		cout << CODE_POS_STR << "错误代码：" << ret << " -- " << ToString((ErrorCode)ret);
	}
}

void video::OutputFormatBase::WriteHeader(AVDictionary **dic)
{
	std::lock_guard l(_not_private_methods_lock);
	int ret = ::avformat_write_header(_wrapped_obj, dic);
	if (ret < 0)
	{
		cout << "write_header 方法异常：" << ToString((ErrorCode)ret) << endl;
		throw jc::Exception();
	}
}

void video::OutputFormatBase::WriteTrailer()
{
	int ret = ::av_write_trailer(_wrapped_obj);
	if (ret < 0)
	{
		cout << CODE_POS_STR << ToString((ErrorCode)ret) << endl;
		throw jc::Exception();
	}
}

bool video::OutputFormatBase::NeedGlobalHeader()
{
	std::lock_guard l(_not_private_methods_lock);
	return _wrapped_obj->oformat->flags & AVFMT_GLOBALHEADER;
}

void video::OutputFormatBase::DumpFormat(char const *url)
{
	std::lock_guard l(_not_private_methods_lock);
	cout << endl;
	cout << "------------------------------------------------------------" << endl;
	cout << "▼ 格式信息" << endl;
	cout << "------------------------------------------------------------" << endl;
	av_dump_format(_wrapped_obj, 0, url, true);
	cout << "------------------------------------------------------------" << endl;
	cout << endl;
}

AVStreamWrapper video::OutputFormatBase::CreateNewStream()
{
	std::lock_guard l(_not_private_methods_lock);
	::AVStream *ps = avformat_new_stream(_wrapped_obj, nullptr);
	if (ps == nullptr)
	{
		cout << CODE_POS_STR << "创建流失败" << endl;
		throw jc::Exception();
	}

	return AVStreamWrapper(ps);
}

AVStreamWrapper video::OutputFormatBase::CreateNewStream(shared_ptr<AVCodecContextWrapper> codec_ctx)
{
	std::lock_guard l(_not_private_methods_lock);
	AVStream *ps = avformat_new_stream(_wrapped_obj, nullptr);
	if (ps == nullptr)
	{
		cout << CODE_POS_STR << "创建流失败" << endl;
		throw jc::Exception();
	}

	AVStreamWrapper stream{ ps };

	/* SetCodecParam 函数设置参数的时候，会将码器的时间基，帧率的信息复制到流中。*/
	int ret = stream.SetCodecParam(codec_ctx);
	if (ret < 0)
	{
		cout << CODE_POS_STR << "设置流参数失败" << endl;
		throw jc::Exception();
	}

	return stream;
}

AVProgramWrapper video::OutputFormatBase::CreateNewProgram()
{
	std::lock_guard l(_not_private_methods_lock);
	/* 节目 ID，用来标识一个节目。
	* - 不是 PID。PID 是 Packet ID 的缩写，不是 Program ID 的缩写。
	* - 从 1 开始，不是从 0 开始。
	*/
	int program_id = _wrapped_obj->nb_programs + 1;
	return AVProgramWrapper{ av_new_program(_wrapped_obj, program_id) };
}

void video::OutputFormatBase::AddStreamToProgram(int program_id, int stream_index)
{
	std::lock_guard l(_not_private_methods_lock);
	av_program_add_stream_index(_wrapped_obj, program_id, stream_index);
}
