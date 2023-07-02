#pragma once
#include<Wraper.h>
#include<AVCodec.h>
#include<AVFrame.h>
#include<include_ffmpeg.h>
#include<AVUtil.h>
#include<AVError.h>

namespace FFmpeg
{
	class AVCodecContext :public Wraper<::AVCodecContext>
	{
		#pragma region 生命周期
	public:
		AVCodecContext() :Wraper() {}
		AVCodecContext(::AVCodecContext *p) :Wraper(p) {}
		AVCodecContext(::AVCodecContext &ref) :Wraper(ref) {}

		~AVCodecContext()
		{
			Dispose();
		}

		void Dispose() override
		{
			if (should_dispose())
			{
				cout << "AVCodecContext 释放" << endl;
				avcodec_free_context(&_pWrapedObj);
			}
		}
		#pragma endregion

		#pragma region 工厂函数
	public:
		/// <summary>
		/// 通过 AVCodec 创建 AVCodecContext
		/// </summary>
		/// <param name="codec"></param>
		/// <returns></returns>
		static FFmpeg::AVCodecContext create(FFmpeg::AVCodec codec)
		{
			FFmpeg::AVCodecContext ctx;
			ctx._codec = codec;
			ctx._pWrapedObj = ::avcodec_alloc_context3(codec);
			if (!ctx._pWrapedObj)
				throw Exception("avcodec_alloc_context3失败");
			return ctx;
		}

		/// <summary>
		/// 通过 AVCodec 创建 AVCodecContext，然后复制指定的 AVCodecParameters 到本类对象中
		/// </summary>
		/// <param name="codec"></param>
		/// <param name="param"></param>
		/// <returns></returns>
		static FFmpeg::AVCodecContext create(FFmpeg::AVCodec codec, AVCodecParameters *param)
		{
			FFmpeg::AVCodecContext ctx = create(codec);
			int ret = ::avcodec_parameters_to_context(ctx._pWrapedObj, param);
			if (ret < 0)
				throw Exception("AVCodecContext::create", ret);
			else
				return ctx;
		}
		#pragma endregion

		#pragma region 包装方法
	public:
		/// <summary>
		/// 打开编码器
		/// </summary>
		/// <param name="dic"></param>
		void open_codec(FFmpeg::AVDictionary dic = nullptr)
		{
			int ret = ::avcodec_open2(_pWrapedObj, _codec, dic);
			if (ret < 0)
				throw Exception("AVCodecContext::open_codec", ret);
		}

		/// <summary>
		/// 将未编码帧送入编码器进行编码。随后可调用 avcodec_receive_packet 方法接收编码完成的包。
		/// </summary>
		/// 
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
		/// 
		/// <param name="pkt">
		/// 提前创建好的包对象。如果是在循环内调用本方法，请在循环外创建包对象，然后重复使用同一个包
		/// 对象，避免频繁的堆内存分配和释放。在循环结束的时候别玩了调用包对象的 unref 方法。
		/// </param>
		/// <exception cref="exception">发生错误会抛出异常</exception>
		void avcodec_receive_packet(FFmpeg::AVPacket pkt)
		{
			int ret = ::avcodec_receive_packet(_pWrapedObj, pkt);
			if (ret < 0)
				throw Exception("AVCodecContext::avcodec_receive_packet", ret);
		}

		/// <summary>
		/// 向编码器发送包（未解码的数据）
		/// </summary>
		/// <param name="packet"></param>
		/// <exception cref="int"></exception>
		void send_packet(FFmpeg::AVPacket packet)
		{
			int ret = ::avcodec_send_packet(_pWrapedObj, packet);
			if (ret < 0)
				throw Exception("AVCodecContext::send_packet", ret);
		}

		/// <summary>
		/// 接收解码后的帧，接收成功返回 0，失败返回错误代码
		/// </summary>
		/// <param name="frame"></param>
		/// <returns></returns>
		int receive_frame(FFmpeg::AVFrame frame)
		{
			return ::avcodec_receive_frame(_pWrapedObj, frame);
		}

		void set_codec_param(AVCodecParameters *param)
		{
			int ret = ::avcodec_parameters_to_context(_pWrapedObj, param);
			if (ret < 0)
				throw Exception("AVCodecContext::set_codec_param", ret);
		}
		#pragma endregion

		#pragma region 私有字段
	private:
		/// <summary>
		/// 与本类绑定的 AVCodec
		/// </summary>
		FFmpeg::AVCodec _codec;
		#pragma endregion
	};
}

