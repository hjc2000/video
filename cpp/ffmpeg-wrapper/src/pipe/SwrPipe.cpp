#include "SwrPipe.h"
#include<ErrorCode.h>

void video::SwrPipe::read_and_send_frame()
{
	while (1)
	{
		int ret = _swr->ReadFrame(_swr_out_frame);
		switch (ret)
		{
		case 0:
			{
				SendFrameToEachConsumer(&_swr_out_frame);

				// 下轮循环继续读取
				break;
			}
		case (int)ErrorCode::output_is_temporarily_unavailable:
			{
				return;
			}
		case (int)ErrorCode::eof:
			{
				SendFrameToEachConsumer(nullptr);
				return;
			}
		default:
			{
				// 未知错误
				throw jc::Exception();
			}
		}
	}
}

void video::SwrPipe::read_and_send_frame_without_flushing_consumer()
{
	while (1)
	{
		int ret = _swr->ReadFrame(_swr_out_frame);
		switch (ret)
		{
		case 0:
			{
				SendFrameToEachConsumer(&_swr_out_frame);

				// 下轮循环继续读取
				break;
			}
		case (int)ErrorCode::output_is_temporarily_unavailable:
			{
				return;
			}
		case (int)ErrorCode::eof:
			{
				return;
			}
		default:
			{
				// 未知错误
				throw jc::Exception();
			}
		}
	}
}
