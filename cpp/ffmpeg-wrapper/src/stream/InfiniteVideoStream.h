#pragma once
#include<BlockingCircleBufferMemoryStream.h>
#include<CustomOutputFormatContext.h>
#include<FileStream.h>
#include<InfiniteBestStreamDemuxDecoder.h>
#include<ProgramEncoderMuxer.h>

namespace video
{
	class InfiniteVideoStream :public Stream
	{
	public:
		InfiniteVideoStream();

		InfiniteVideoStream(List<std::string> format_list, bool loop);

		~InfiniteVideoStream()
		{
			Close();
		}

	private:
		List<std::string> _format_list;
		bool _loop = false;
		int _format_index = 0;
		shared_ptr<InfiniteBestStreamDemuxDecoder_old> _best_stream_demux_decoder;
		shared_ptr<jc::BlockingCircleBufferMemoryStream> _buffer_memory_stream{
			new jc::BlockingCircleBufferMemoryStream{1024 * 64}
		};

		void Init();
		shared_ptr<InputFormatContext> OnGetFormatCallback();

	public:
		#pragma region 通过 Stream 继承
		bool CanRead() override;
		bool CanWrite() override;
		bool CanSeek() override;
		int64_t Length() override;
		void SetLength(int64_t value) override;
		int64_t Read(uint8_t *dst_buf, int64_t offset, int64_t count) override;
		void Write(uint8_t *src_buf, int64_t offset, int64_t count) override;
		void Flush() override;
		void Close() override;
		int64_t Position() override;
		void SetPosition(int64_t value) override;
		#pragma endregion

	};
}