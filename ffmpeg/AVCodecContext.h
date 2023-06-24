#pragma once
#include<Wraper.h>
#include<AVCodec.h>
#include<AVFrame.h>
#include<include_ffmpeg.h>

namespace FFmpeg
{
	class AVCodecContext :public Wraper<::AVCodecContext>
	{
	public:// 生命周期
		/// <summary>
		/// 通过编码器创建一个编码器上下文
		/// </summary>
		/// <param name="codec"></param>
		AVCodecContext(FFmpeg::AVCodec codec) :_codec(codec)
		{
			_pWrapedObj = ::avcodec_alloc_context3(codec);
			if (!_pWrapedObj)
				throw "avcodec_alloc_context3失败";
		}
		~AVCodecContext()
		{
			if (should_dispose())
			{
				cout << "AVCodecContext 析构" << endl;
				avcodec_free_context(&_pWrapedObj);
			}
		}

	public://公共方法
		/// <summary>
		/// 打开编码器
		/// </summary>
		/// <param name="dic"></param>
		void avcodec_open2(FFmpeg::AVDictionary dic = nullptr)
		{
			int ret = ::avcodec_open2(_pWrapedObj, _codec, dic);
			if (ret < 0)
				throw ret;
		}

		/// <summary>
		/// 将未编码帧送入编码器进行编码。随后可调用 avcodec_receive_packet 方法接收编码完成的包。
		/// </summary>
		/// <param name="frame"></param>
		void avcodec_send_frame(FFmpeg::AVFrame frame)
		{
			::avcodec_send_frame(_pWrapedObj, frame);
		}

		/*
		
		enc_ctx.avcodec_send_frame(frame);
		FFmpeg::AVPacket pkt;
		while (1)
		{
			try
			{
				enc_ctx.avcodec_receive_packet(pkt);
				FFmpeg::AVPacket pkt1;
				fwrite(pkt()->data, 1, pkt()->size, outfile);
			}
			catch (int err_code)
			{
				if (err_code == AVERROR(EAGAIN) || err_code == AVERROR_EOF)
					return;
				else if (err_code < 0)
					throw err_code;
			}

			pkt.unref();
		}

		*/
		/// <summary>
		/// 接收编码完成的包。
		/// * 在死循环内接收。当发生错误或需要送入新的未编码帧时本方法会抛出异常。
		/// * 在 catch 块中接收 int 类型的错误代码。当错误代码值等于 AVERROR(EAGAIN) 或
		///	AVERROR_EOF 时是正常现象。
		/// * AVERROR(EAGAIN) 错误码意味着需要送入新的未编码帧才能继续读出编码完成的包。
		/// * 如果是其他错误代码，说明发生了不正常的错误，需要重新抛出这个错误代码。
		/// * 示例见上方紧跟着的块注释
		/// </summary>
		/// <param name="pkt">
		/// 提前创建好的包对象。如果是在循环内调用本方法，请在循环外创建包对象，然后重复使用同一个包
		/// 对象，避免频繁的堆内存分配和释放。在循环结束的时候别玩了调用包对象的 unref 方法。
		/// </param>
		void avcodec_receive_packet(FFmpeg::AVPacket pkt)
		{
			int ret = ::avcodec_receive_packet(_pWrapedObj, pkt);
			if (ret < 0)
				throw ret;
		}

	private:
		/// <summary>
		/// 保存构造函数传进来的编码器参数
		/// </summary>
		FFmpeg::AVCodec _codec;
	};
}

