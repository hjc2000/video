#pragma once
#include<Wraper.h>
#include<AVCodec.h>
#include<AVFrame.h>
#include<include_ffmpeg.h>
#include<AVUtil.h>
#include<AVError.h>
#include<AVPacket.h>
#include<AVDictionary.h>

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
			{
				throw Exception("FFmpeg::AVCodecContext create(FFmpeg::AVCodec codec) 失败");
			}

			return ctx;
		}

		/// <summary>
		/// 通过 AVCodec 创建 AVCodecContext，然后复制指定的 AVCodecParameters 到本类对象中
		/// </summary>
		/// <param name="codec">编解码器</param>
		/// <param name="param">编解码器参数</param>
		/// <param name="autoOpen">创建编解码器上下文完成后自动打开</param>
		/// <returns>返回创建完成的编解码器上下文</returns>
		static FFmpeg::AVCodecContext create(FFmpeg::AVCodec codec, AVCodecParameters *param, bool autoOpen = false)
		{
			FFmpeg::AVCodecContext ctx = create(codec);
			int ret = ::avcodec_parameters_to_context(ctx._pWrapedObj, param);
			if (ret < 0)
			{
				throw Exception("AVCodecContext::create", ret);
			}
			else
			{
				if (autoOpen)
				{
					ctx.open();
				}

				return ctx;
			}
		}
		#pragma endregion

		#pragma region 包装方法
	public:
		/// <summary>
		/// 打开编解码器。注意，打开前一定要设置编码器上下文的参数。要么调用
		/// FFmpeg::AVCodecContext create(FFmpeg::AVCodec codec) 
		/// 创建一个编解码器上下文后手动赋值来设置，要么使用
		///	static FFmpeg::AVCodecContext create(FFmpeg::AVCodec codec, AVCodecParameters *param, bool autoOpen = false)
		/// 来根据 AVCodecParameters 创建一个初始化完参数的编解码器。
		/// 当然，也可以随时调用 set_codec_param 来设置编解码器上下文参数。
		/// </summary>
		/// <param name="dic"></param>
		void open(FFmpeg::AVDictionary dic = nullptr);

		/// <summary>
		/// 将未编码帧送入编码器进行编码。随后可调用 avcodec_receive_packet 方法接收编码完成的包。
		/// </summary>
		/// 
		/// <param name="frame"></param>
		void avcodec_send_frame(FFmpeg::AVFrame frame)
		{
			::avcodec_send_frame(_pWrapedObj, frame);
		}

		/// <summary>
		/// 从编码器接受包。成功返回 0，失败返回错误代码
		/// </summary>
		/// <param name="pkt"></param>
		/// <returns></returns>
		int avcodec_receive_packet(FFmpeg::AVPacket pkt)
		{
			return ::avcodec_receive_packet(_pWrapedObj, pkt);
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
			{
				throw Exception("AVCodecContext::send_packet", ret);
			}
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
			{
				throw Exception("AVCodecContext::set_codec_param", ret);
			}
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

