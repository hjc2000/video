#pragma once
#include<Wraper.h>
#include<AVCodec.h>
#include<include_ffmpeg.h>

namespace FFmpeg
{
	class AVFrame;
	class AVPacket;
	class AVDictionary;

	class AVCodecContext :public Wraper<::AVCodecContext>
	{
		#pragma region 生命周期
	public:
		AVCodecContext() :Wraper() {}
		AVCodecContext(::AVCodecContext *p) :Wraper(p) {}
		AVCodecContext(::AVCodecContext &ref) :Wraper(ref) {}
		AVCodecContext(FFmpeg::AVCodec codec);
		AVCodecContext(FFmpeg::AVCodec codec, AVCodecParameters *param, bool autoOpen = false);
		~AVCodecContext() { Dispose(); }
		void Dispose() override;
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
		void open(FFmpeg::AVDictionary *dic = nullptr);

		/// <summary>
		/// 将未编码帧送入编码器进行编码。随后可调用 avcodec_receive_packet 方法接收编码完成的包。
		/// </summary>
		/// 
		/// <param name="frame"></param>
		void avcodec_send_frame(FFmpeg::AVFrame frame);

		/// <summary>
		/// 从编码器接受包。成功返回 0，失败返回错误代码
		/// </summary>
		/// <param name="pkt"></param>
		/// <returns></returns>
		int avcodec_receive_packet(FFmpeg::AVPacket pkt);

		/// <summary>
		/// 向编码器发送包（未解码的数据）
		/// </summary>
		/// <param name="packet"></param>
		/// <exception cref="int"></exception>
		void send_packet(FFmpeg::AVPacket packet);

		/// <summary>
		/// 接收解码后的帧，接收成功返回 0，失败返回错误代码
		/// </summary>
		/// <param name="frame"></param>
		/// <returns></returns>
		int receive_frame(FFmpeg::AVFrame frame);

		void set_codec_param(AVCodecParameters *param);
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

