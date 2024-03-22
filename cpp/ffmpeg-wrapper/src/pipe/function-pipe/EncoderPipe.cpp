#include"EncoderPipe.h"
#include<ErrorCode.h>

using namespace video;

EncoderPipe::EncoderPipe(
	char const *codec_name,
	IVideoStreamInfoCollection &in_stream_infos,
	shared_ptr<OutputFormat> output_format,
	int64_t out_bit_rate_in_bps
)
{
	_output_format = output_format;
	_encoder_ctx = AVCodecContextWrapper::CreateEncoder(
		codec_name,
		in_stream_infos,
		output_format->NeedGlobalHeader(),
		false
	);

	if (out_bit_rate_in_bps > 0)
	{
		// 如果传入了有效值，则设置 bit_rate 字段。
		(*_encoder_ctx)->bit_rate = out_bit_rate_in_bps;
	}

	(*_encoder_ctx)->gop_size = in_stream_infos.frame_rate().num / in_stream_infos.frame_rate().den;

	//// 设置色彩空间为 Rec.2020
	//(*_encoder_ctx)->colorspace = AVCOL_SPC_BT2020_NCL;

	//// 色彩原色设置
	//(*_encoder_ctx)->color_primaries = AVCOL_PRI_BT2020;

	//// 转移特性，例如使用 PQ
	//(*_encoder_ctx)->color_trc = AVCOL_TRC_SMPTE2084;

	_encoder_ctx->Open();
	_new_stream = output_format->CreateNewStream(_encoder_ctx);
}

EncoderPipe::EncoderPipe(
	char const *codec_name,
	IAudioStreamInfoCollection &in_stream_infos,
	shared_ptr<OutputFormat> output_format
)
{
	_output_format = output_format;
	_encoder_ctx = AVCodecContextWrapper::CreateEncoder(
		codec_name,
		in_stream_infos,
		output_format->NeedGlobalHeader()
	);

	_new_stream = output_format->CreateNewStream(_encoder_ctx);
}

void EncoderPipe::ReadAndSendPacketToOutputFormat()
{
	AVPacketWrapper packet;
	while (1)
	{
		int ret = _encoder_ctx->ReadPacket(packet);
		switch (ret)
		{
		case 0:
			{
				packet.set_stream_index(_new_stream.Index());
				_output_format->SendPacket(&packet);

				// 下一轮循环继续读取包
				break;
			}
		case (int)ErrorCode::output_is_temporarily_unavailable:
			{
				return;
			}
		case (int)ErrorCode::eof:
			{
				_output_format->SendPacket(nullptr);
				return;
			}
		default:
			{
				// 未知错误
				cout << CODE_POS_STR << "read_packet 返回了未知的错误代码" << endl;
				throw jc::Exception();
			}
		}
	}
}