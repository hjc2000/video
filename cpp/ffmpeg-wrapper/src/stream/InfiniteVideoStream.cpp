#include"InfiniteVideoStream.h"

using namespace video;

video::InfiniteVideoStream::InfiniteVideoStream()
{
	_format_list.Add("idol.ts");
	_format_list.Add("渡尘.mp4");
	_format_list.Add("moon.mp4");
	_format_list.Add("越权访问.mkv");
	_format_list.Add("zf.ts");
	_format_list.Add("水龙吟.ts");
	_format_list.Add("崩坏世界的歌姬.ts");
	_format_list.Add("不老梦.ts");
	_loop = true;
	Init();
}

video::InfiniteVideoStream::InfiniteVideoStream(List<std::string> format_list, bool loop)
{
	_format_list = format_list;
	_loop = loop;
	Init();
}

void video::InfiniteVideoStream::Init()
{
	// 节目解封装、解码管道
	auto get_format_callback = [&]()->shared_ptr<InputFormatContext>
	{
		return OnGetFormatCallback();
	};

	_best_stream_demux_decoder = shared_ptr<InfiniteBestStreamDemuxDecoder>{
		new InfiniteBestStreamDemuxDecoder{
			get_format_callback
		}
	};

	// 节目编码、封装管道
	shared_ptr<AVIOContextWrapper> out_io_ctx{
		new AVIOContextWrapper{
			true,
			_buffer_memory_stream
		}
	};

	shared_ptr<CustomOutputFormatContext> out_fmt_ctx{
		new CustomOutputFormatContext{
			"mux_out.ts",
			out_io_ctx
		}
	};
	out_fmt_ctx->_on_all_stream_flushed_async = [&]()
	{
		Close();
	};

	shared_ptr<ProgramEncoderMuxer> program_encode_muxer{ new ProgramEncoderMuxer{out_fmt_ctx} };
	program_encode_muxer->AddMultiStream(
		_best_stream_demux_decoder,
		"hevc_amf",
		"aac",
		44100
	);

	// 开始数据传输
	program_encode_muxer->WriteHeader();
	_best_stream_demux_decoder->StartPump();
}

shared_ptr<InputFormatContext> video::InfiniteVideoStream::OnGetFormatCallback()
{
	if (_format_index >= _format_list.Count())
	{
		cout << "没有更多封装，结束" << endl;
		return nullptr;
	}

	shared_ptr<InputFormatContext> in_fmt_ctx{
		new InputFormatContext{
			_format_list[_format_index]
		}
	};

	in_fmt_ctx->DumpFormat();
	_format_index++;
	if (_format_index >= _format_list.Count() && _loop)
	{
		_format_index = 0;
	}

	return in_fmt_ctx;
}

bool video::InfiniteVideoStream::CanRead()
{
	return true;
}

bool video::InfiniteVideoStream::CanWrite()
{
	return false;
}

bool video::InfiniteVideoStream::CanSeek()
{
	return false;
}

int64_t video::InfiniteVideoStream::Length()
{
	throw jc::NotSupportedException();
}

void video::InfiniteVideoStream::SetLength(int64_t value)
{
	throw jc::NotSupportedException();
}

int64_t video::InfiniteVideoStream::Read(uint8_t *dst_buf, int64_t offset, int64_t count)
{
	return _buffer_memory_stream->Read(dst_buf, offset, count);
}

void video::InfiniteVideoStream::Write(uint8_t *src_buf, int64_t offset, int64_t count)
{
	throw jc::NotSupportedException();
}

void video::InfiniteVideoStream::Flush()
{
}

void video::InfiniteVideoStream::Close()
{
	/* 一定要先把 _buffer 给关闭了，让它不再阻塞，这样才能避免 _best_stream_demux_decoder
	 * 因为被 _buffer 阻塞住了，进而导致 Dispose 方法被阻塞。
	 */
	_buffer_memory_stream->Close();
	_best_stream_demux_decoder->Dispose();
}

int64_t video::InfiniteVideoStream::Position()
{
	throw jc::NotSupportedException();
}

void video::InfiniteVideoStream::SetPosition(int64_t value)
{
	throw jc::NotSupportedException();
}
