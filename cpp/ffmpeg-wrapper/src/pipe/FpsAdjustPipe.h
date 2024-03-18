#pragma once
#include<AVCalculate.h>
#include<AVError.h>
#include<List.h>
#include<PipeFrameSource.h>
#include<Queue.h>
#include<Trigger.h>
#include<VideoFilterGraph.h>
#include<Wrapper.h>
#include<include_ffmpeg.h>

namespace video
{
	/**
	 * @brief 用来调整帧率的管道。帧从这里输入后再输出，帧率就变了。这里会插帧或删帧。
	 * 需要注意：改变帧率并不会提升视频质量，反而会降低。
	*/
	class FpsAdjustPipe :
		public IFrameConsumer,
		public PipeFrameSource
	{
	public:
		/**
		 * @brief
		 * @param input_video_stream_infos 输入的视频流信息。
		 * @param desired_out_fps 期望的输出帧率。
		*/
		FpsAdjustPipe(IVideoStreamInfoCollection &input_video_stream_infos, AVRational desired_out_fps) :
			_input_video_stream_infos(input_video_stream_infos),
			_graph(input_video_stream_infos),
			_desired_out_fps(desired_out_fps)
		{
			auto fps_filter = _graph.alloc_fps_filter(desired_out_fps);

			// 连接滤镜
			_graph.buffer_filter() << fps_filter << _graph.buffer_sink_filter();

			// 配置滤镜图
			_graph.config_graph();
		}

	private:
		VideoStreamInfoCollection _input_video_stream_infos;
		VideoFilterGraph _graph;
		AVRational _desired_out_fps;
		jc::Trigger<int64_t> _trigger;

		void read_and_send_frame()
		{
			AVFrameWrapper frame;
			bool have_synced = false;
			int64_t delta_pts = 0;

			while (1)
			{
				int ret = _graph.ReadFrame(frame);
				switch (ret)
				{
				case 0:
					{
						// 将读取的帧的时间戳与 _ref_pts 对齐。
						if (!have_synced)
						{
							int64_t ref_pts = _trigger.GetOutput();

							/* 将触发器的输出更新到输入。
							 * 从 fps 滤镜输出端读取帧成功的那一刻，fps 滤镜输入端送入的那个帧就是下一次
							 * 读取成功时要同步 pts 的对象。
							 */
							_trigger.UpdateOutput();
							delta_pts = ref_pts - frame.pts();
							have_synced = true;
						}

						// 滤镜出来的 pts 与输入端的 pts 有误差，则本轮循环读取的每一个帧的 pts 都要加上 delta_pts。
						frame.set_pts(frame.pts() + delta_pts);
						frame.set_time_base(AVRational{ _desired_out_fps.den, _desired_out_fps.num });
						SendFrameToEachConsumer(&frame);

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
				}
			}
		}

	public:
		/// <summary>
		///		* 向 fps 调整管道送入帧。处理完后帧会穿过管道，送给消费者。
		///		离开管道的帧的时间基为 1/fps。此外，输出帧的 pts 会与输入帧进行同步，防止
		///		输入端的 pts 跳跃后输出端仍然保持 pts 的连续，这将会导致音视频不同步。
		/// 
		///		* 如果输出端没有任何消费者，则本函数会直接返回，不执行任何工作。
		/// </summary>
		/// <param name="frame"></param>
		void SendFrame(AVFrameWrapper *frame) override
		{
			if (_consumer_list.Count() == 0)
			{
				return;
			}

			read_and_send_frame();
			_graph.SendFrame(frame);

			if (frame)
			{
				int64_t rescaled_current_frame_pts = ConvertTimeStamp(
					frame->pts(),
					frame->time_base(),
					AVRational{ _desired_out_fps.den, _desired_out_fps.num }
				);

				_trigger.UpdateInput(rescaled_current_frame_pts);
				if (_trigger.HaveNotUpdateOutput())
				{
					// 第一次输入数据，让输入的数据直通到输出端
					_trigger.UpdateOutput();
				}
			}

			read_and_send_frame();
		}
	};
}