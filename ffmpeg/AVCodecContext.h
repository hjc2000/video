#pragma once
#include<Wraper.h>
#include<AVCodec.h>
#include<AVFrame.h>
#include<include_ffmpeg.h>

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
		/// 通过 AVCodec 创建
		/// </summary>
		/// <param name="codec"></param>
		/// <returns></returns>
		static FFmpeg::AVCodecContext create(FFmpeg::AVCodec codec)
		{
			FFmpeg::AVCodecContext ctx;
			ctx._codec = codec;
			ctx._pWrapedObj = ::avcodec_alloc_context3(codec);
			if (!ctx._pWrapedObj)
				throw "avcodec_alloc_context3失败";
			return ctx;
		}

		/// <summary>
		/// 通过 AVCodec 创建，然后复制指定的 AVCodecParameters 到本类对象中
		/// </summary>
		/// <param name="codec"></param>
		/// <param name="param"></param>
		/// <returns></returns>
		static FFmpeg::AVCodecContext create(FFmpeg::AVCodec codec, AVCodecParameters *param)
		{
			FFmpeg::AVCodecContext ctx = create(codec);
			int ret = ::avcodec_parameters_to_context(ctx._pWrapedObj, param);
			if (ret < 0)
			{
				cout << "设置编解码器参数失败" << endl;
				throw ret;
			}
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

		/// <summary>
		/// 向编码器发送包（未解码的数据）
		/// </summary>
		/// <param name="packet"></param>
		void send_packet(FFmpeg::AVPacket packet)
		{
			int ret = ::avcodec_send_packet(_pWrapedObj, packet);
			if (ret < 0)
			{
				cout << "向编码器发送包失败" << endl;
				throw ret;
			}
		}

		/// <summary>
		/// * 从编码器中接收帧。向编码器发送一个包后，可以重复调用本方法读取解码出来的帧，因为
		/// 一个包可能对应多个帧。
		/// * 如果读取出帧，返回 true，如果没有读取出帧，表明编码器内没有
		/// 有效的帧了，需要再次发送一个包，此时返回 false。
		/// * 如果发生解码错误，会抛出异常。
		/// * 可以将本方法放在循环语句的括号中作为循环条件
		/// </summary>
		/// <param name="frame"></param>
		/// <returns></returns>
		bool receive_frame(FFmpeg::AVFrame frame)
		{
			int ret = avcodec_receive_frame(_pWrapedObj, frame);
			if (ret < 0)
			{
				// 这两个错误表示编码器中没有有效的帧了，需要再次送入一个包
				if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
					return false;
				else
					throw ret;
			}
			else
			{
				return true;
			}
		}

		void set_codec_param(AVCodecParameters *param)
		{
			int ret = ::avcodec_parameters_to_context(_pWrapedObj, param);
			if (ret < 0)
				throw ret;
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

