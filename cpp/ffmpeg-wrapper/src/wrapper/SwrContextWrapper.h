#pragma once
#include<AVError.h>
#include<AVFrameWrapper.h>
#include<AudioFrameInfoCollection.h>
#include<AudioStreamInfoCollection.h>
#include<IAudioStreamInfoCollection.h>
#include<IFrameConsumer.h>
#include<IFrameSource.h>
#include<atomic>
#include<include_ffmpeg.h>
#include<mutex>
#include<thread>

namespace video
{
	using namespace std;

	/// <summary>
	///		重采样器。
	///		* 本类线程安全，因为所有的公共方法都加了互斥锁。
	/// </summary>
	class SwrContextWrapper :
		public Wrapper<SwrContext>,
		public IFrameConsumer,
		public IFrameSource
	{
		#pragma region 生命周期
	public:
		/// <summary>
		///		使用 IAudioStreamInfoCollection 接口初始化重采样器。
		///		* 因为重采样器要为输入输出的帧打上时间戳，所以要求 in_stream_infos 和 out_frame_infos
		///		的时间基有效。
		/// </summary>
		/// 
		/// <param name="in_stream_infos">
		///		设置重采样器输入流。
		/// </param>
		/// <param name="out_frame_infos">
		///		设置重采样器输出流。
		/// </param>
		SwrContextWrapper(
			IAudioStreamInfoCollection &in_stream_infos,
			IAudioFrameInfoCollection &out_frame_infos
		)
		{
			_in_stream_infos = in_stream_infos;
			_out_frame_infos = out_frame_infos;

			int ret = swr_alloc_set_opts2(
				&_wrapped_obj,
				&_out_frame_infos._ch_layout,
				_out_frame_infos._sample_format,
				_out_frame_infos._sample_rate,
				&_in_stream_infos._ch_layout,
				_in_stream_infos._sample_format,
				_in_stream_infos._sample_rate,
				0,
				nullptr
			);

			if (ret < 0)
			{
				cout << CODE_POS_STR << "构造重采样器并设置参数失败" << endl;
				throw jc::Exception();
			}

			ret = swr_init(_wrapped_obj);
			if (ret < 0)
			{
				cout << CODE_POS_STR << "初始化重采样器失败" << endl;
				throw jc::Exception();
			}
		}

		~SwrContextWrapper()
		{
			cout << "SwrContextWrapper 析构函数" << endl;
			swr_free(&_wrapped_obj);
		}
		#pragma endregion

	private:
		/// <summary>
		///		非私有的方法，即公共的和保护的，都要加锁。
		///		* 为什么保护的要加锁？因为保护的有可能是继承了基类的，而基类可能用公共方法
		///		  去调用保护的方法，导致本类的保护方法和公共方法一样，都有可能被外部在不同
		///		  线程中调用。
		/// </summary>
		recursive_mutex _not_private_methods_lock;

		/// <summary>
		///		send_frame 方法被送入空指针，开启了冲洗模式。
		/// </summary>
		atomic_bool _flushed = false;

		AudioStreamInfoCollection _in_stream_infos;
		AudioFrameInfoCollection _out_frame_infos;

	private:
		/// <summary>
		///		对 swr_convert 的简单封装。
		/// </summary>
		/// <param name="out">接收输出数据的缓冲区</param>
		/// <param name="out_count">想要接收多少个采样点</param>
		/// <param name="in">要被重采样的采样点的缓冲区</param>
		/// <param name="in_count">
		///		要被重采样的采样点个数（单个声道的采样点个数。所有声道的采样点个数必须相同）
		/// </param>
		/// <returns>
		///		成功则返回实际被写入 out 中的采样点个数。失败返回负数的错误代码。
		/// </returns>
		int convert(uint8_t **out, int out_count, uint8_t **in, int in_count)
		{
			int ret = swr_convert(
				_wrapped_obj,
				out,
				out_count,
				(const uint8_t **)in,
				in_count
			);

			return ret;
		}

		/// <summary>
		///		send_frame 方法被调用，向重采样器送入帧时就会记录那个帧的 pts。
		/// 
		///		_in_pts_when_send_frame 相当于未来的时间戳。只要将重采样缓冲区中的采样点播放完，
		///		重采样器输出端的时间戳就会真正变成 _in_pts_when_send_frame。也就是输入端的时间戳
		///		和输出端的时间戳不仅时间基不一样，还存在时间差。
		/// </summary>
		int64_t _in_pts_when_send_frame = 0;

		#pragma region ReadFrame 方法专用
		/**
		 * @brief 对 input_frame 缓冲区的音频帧进行重采样，输出到 output_frame 的缓冲区中。
		 *
		 * @param input_frame 要被重采样的帧。可以传入空指针，此时表示不输入数据。
		 *
		 * @param output_frame 重采样输出帧。
		 * - 注意，此帧可能不完整，因为有可能发生实际写入 output_frame 缓冲区的采样点个数小于 output_frame
		 *   的 nb_samples 属性的情况。实际写入的采样点个数见本函数的返回值。
		 * - 可以传入空指针，此时表示不输出数据。如果 output_frame 为空指针，input_frame 不为空指针，
		 *   则表示只输入数据不输出数据。此时重采样的结果会储存在重采样器内部的缓冲区中。可以之后在
		 *	 input_frame 传入空指针，output_frame 传入非空指针来冲洗缓冲区。
		 *
		 * @return 实际写入 output_frame 缓冲区的可用的采样点个数。失败返回负数的错误代码。
		*/
		int convert(AVFrameWrapper *input_frame, AVFrameWrapper *output_frame)
		{
			uint8_t **in = input_frame ? (*input_frame)->extended_data : nullptr;
			int in_count = input_frame ? input_frame->nb_samples() : 0;

			uint8_t **out = output_frame ? (*output_frame)->extended_data : nullptr;
			int out_count = output_frame ? output_frame->nb_samples() : 0;

			// 重采样
			int ret = convert(out, out_count, in, in_count);

			return ret;
		}

		int read_frame_in_flushing_mode(AVFrameWrapper &output_frame)
		{
			int count = convert(nullptr, &output_frame);
			cout << "冲洗缓冲区冲洗出了" << count << "个采样点" << endl;

			// 如果有填充（即 count > 0）且填充了不完整的帧
			if (count > 0 && count < output_frame.nb_samples())
			{
				// 将后面没被填充的采样点设置为静音
				output_frame.mute(count);
				cout << "填充了不完整的帧，已将后面未填充的采样点设置为静音" << endl;
			}

			if (count > 0)
			{
				/* 有填充，说明重采样器内还是有采样点的，返回 0，表示读取成功。
				* 只不过这个读取成功的帧有可能尾部是被填充了静音采样点的。
				*/
				return 0;
			}

			// 一个采样点都没填充，本次读取帧失败，返回 eof。
			return (int)ErrorCode::eof;
		}

		int read_frame_in_non_flushing_mode(AVFrameWrapper &output_frame)
		{
			if (can_fill_output_frame(output_frame))
			{
				// 可以填充一个完整的帧
				int count = convert(nullptr, &output_frame);
				if (count != output_frame.nb_samples())
				{
					throw FFmpegException("read_frame 没有填充完整的 output_frame，本来认为这里一定会填充完整的帧的。");
				}

				return 0;
			}

			// 无法填充一个完整的帧，所以一个采样点都不填充
			// 返回 -11 表示输出暂时不可用，需要更多的输入数据。这是官方的错误代码。
			return (int)ErrorCode::output_is_temporarily_unavailable;
		}
		#pragma endregion

		shared_ptr<AVFrameWrapper> _silence_frame;

	public:
		/// <summary>
		///		获取重采样器内部的延迟。
		/// 
		///		重采样器内部有一个高阶滤波器，高阶滤波器需要不同时刻的采样值来做差分，
		///		所以内部有一个采样点队列用来储存不同时刻的采样值。这会导致输出产生延迟。
		/// 
		///		此外，如果转换完的采样点没有被取走，则会被储存在内部的输出缓冲区中，这
		///		也会产生延迟。此函数用来获取所有的这些延迟。
		/// </summary>
		/// <param name="base">
		///		延迟的基。（不是时间基，base 是时间基的倒数。详见 https://www.yuque.com/qiaodangyi/yrin4p/ica2heo5g3kyre9t）
		///		
		///		* 使用时间基的倒数是因为 ffmpeg 默认你会取一个 (0, 1] 内的，并且很接近 0 的数作为时间基，因为
		///		这样精度才高。整型无法表示这种数，所以取了倒数。不用 double 是因为 double 在表示很小的数，例如
		///		1/ 90000 时精度不能满足要求。这种时候用整型可以没有误差。time_base = 1 / 90000，则 base = 90000。
		/// </param>
		/// <returns>
		///		返回值 = 延迟时间 * base
		/// </returns>
		int64_t get_delay(int64_t base)
		{
			lock_guard l(_not_private_methods_lock);
			return swr_get_delay(_wrapped_obj, base);
		}

		/// <summary>
		///		根据构造时传入的输出采样率来计算延迟为多少个输出采样点
		/// </summary>
		/// <returns>延迟为多少个输出采样点</returns>
		int get_delay_as_out_nb_samples()
		{
			// get_delay 已经加锁了，这里不用加锁。
			return (int)get_delay(_out_frame_infos._sample_rate);
		}

		/// <summary>
		///		估算当向此重采样器输入 in_nb_samples 数量的采样点时，输出将会是多少采样点。
		///		此估计值会比实际值偏大，因此可以用来分配一个足够大的缓冲区
		/// </summary>
		/// <param name="in_nb_samples">将要向重采样器输入多少采样点</param>
		/// <returns>将会输出多少采样点</returns>
		/// <note>
		///		相关原理可以查看 https://www.yuque.com/qiaodangyi/yrin4p/dgyse5lne123ovav
		///		本来我是根据这个原理手动写一个函数的，但是现在根据 swr_get_out_samples 了。
		///		因为 swr_get_out_samples 计算出来的值比我手写的那个还大。不知道还有什么机制没了解的。
		/// </note>
		int get_out_nb_samples(int in_nb_samples)
		{
			lock_guard l(_not_private_methods_lock);
			int samples = swr_get_out_samples(_wrapped_obj, in_nb_samples);
			if (samples < 0)
			{
				throw FFmpegException("swr_get_out_samples 函数出错", samples);
			}

			return samples;
		}

		/// <summary>
		///		检查重采样器输出缓冲区中的数据是否足够填满 output_frame
		/// </summary>
		/// <param name="output_frame"></param>
		/// <returns>
		///		足够填满则返回 true，不够则返回 false。
		///	</returns>
		bool can_fill_output_frame(AVFrameWrapper &output_frame)
		{
			// get_delay_as_out_nb_samples 是公共方法，已经加锁了，所以这里不用加锁。
			return get_delay_as_out_nb_samples() > output_frame.nb_samples() + 50;
		}

		/// <summary>
		///		是否已启动冲洗模式。
		///		* 在调用 send_frame 方法时传入空指针就会设置此标志位，下一次调用 read_frame 方法将会
		///		以冲洗的方式读出重采样器内的数据。
		/// </summary>
		/// <returns></returns>
		bool Flushed()
		{
			return _flushed;
		}

		/**
		 * @brief 向重采样器送入帧。
		 * @param input_frame 要输入重采样器的帧。可以传入空指针。传入空指针会启动冲洗模式。
		 *
		 * @exception SendFrameException
		*/
		void SendFrame(AVFrameWrapper *input_frame) override;
		bool _have_printed_send_frame_rescale_pts_warning = false;

		/**
		 * @brief 将重采样器中的数据取出来
		 *
		 * @param output_frame 用于接收重采样输出的数据。接收成功后 output_frame
		 * 的时间基会被设置为初始化本重采样器时的时间基，同时将为 output_frame 打上时间戳。
		 *
		 * @return 读取成功返回 0，失败返回错误代码。
		 * 非冲洗模式：
		 * - 如果成功读取到完整的帧，则返回 0
		 * - 如果重采样器内部的数据不足以填充完整的帧，返回 ErrorCode::output_is_temporarily_unavailable，
		 *   表示需要更多输入数据。
		 *
		 * 冲洗模式：
		 * - 如果重采样器内有数据，则有多少拿多少，直到填充满输出帧或者拿完了。如果拿完后仍然不够填满输出帧，
		 *   则会将输出帧后面没被填充的空隙填充为静音。此时也是返回 0
		 * - 如果重采样器内部没有数据，则返回 ErrorCode::eof，表示到达文件尾。
		*/
		int ReadFrame(AVFrameWrapper &output_frame);

		/// <summary>
		///		向重采样器中送入指定数量的静音采样点。
		/// </summary>
		/// <param name="nb_samples">想要送入多少个静音采样点。</param>
		/// <returns>成功返回 0，失败返回负数的错误代码。</returns>
		int send_silence_samples(uint32_t nb_samples)
		{
			lock_guard l(_not_private_methods_lock);
			if (nb_samples == 0)
			{
				return 0;
			}

			if (!_silence_frame)
			{
				_silence_frame = shared_ptr<AVFrameWrapper>{ new AVFrameWrapper{_in_stream_infos,64} };
				_silence_frame->mute(0);
			}

			// 循环次数
			uint32_t loop_times = nb_samples / _silence_frame->nb_samples();
			for (uint32_t i = 0; i < loop_times; i++)
			{
				SendFrame(_silence_frame.get());
			}

			// 求模，取余数，看用 _silence_frame 填充 loop_times 次后还会剩下几个采样点才能达到 nb_samples
			uint32_t remain_nb_samples = nb_samples % _silence_frame->nb_samples();
			int ret = convert(nullptr,
				0,
				(*_silence_frame)->extended_data,
				remain_nb_samples);
			if (ret < 0)
			{
				// 失败返回负数的错误代码
				return ret;
			}

			// 成功返回 0.
			return 0;
		}
	};
}
