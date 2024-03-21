#pragma once
#include<AVCalculate.h>
#include<IFrameConsumer.h>
#include<PipeFrameSource.h>

namespace video
{
	class PtsAdjustPipe :public IFrameConsumer, public PipeFrameSource
	{
	private:
		/// <summary>
		///		pts 的偏移量，时间基 1/90000. 送入的帧的 pts 会被加上此偏移量。
		/// </summary>
		int64_t _pts_offset = 0;

		/// <summary>
		///		最新的两个帧之间的 pts 间隔。时间基 1/90000.
		/// </summary>
		int64_t _last_two_frame_pts_interval = 0;
		/// <summary>
		///		上一次调用 send_frame 送入的帧的 pts，时间基 1/90000，无论原来的帧的时间基是什么，都要换算为 1/90000，
		///		然后才能赋值给此字段。
		/// </summary>
		int64_t _last_frame_pts = 0;
		/// <summary>
		///		在将送入的帧的 pts 加上偏移量之前调用本函数。
		/// </summary>
		/// <param name="frame"></param>
		void UpdateLastFramePtsBeforeAddingOffset(AVFrameWrapper &frame)
		{
			int64_t now_pts = ConvertTimeStamp(frame.pts(), frame.time_base(), AVRational{ 1,90000 });
			int64_t interval = now_pts - _last_frame_pts;
			if (interval > 0)
			{
				_last_two_frame_pts_interval = interval;
			}

			_last_frame_pts = now_pts;
		}

	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="frame">送入空指针会直接返回，不会冲洗消费者。冲洗消费者需要调用 EndStream 方法。</param>
		void SendFrame(AVFrameWrapper *frame) override
		{
			if (!frame)
			{
				return;
			}

			UpdateLastFramePtsBeforeAddingOffset(*frame);
			frame->set_pts(frame->pts() + _pts_offset);
			SendFrameToEachConsumer(frame);
		}

		/// <summary>
		///		向消费者送入空指针，冲洗他们，表示流结束了。
		/// </summary>
		void EndStream()
		{
			SendFrameToEachConsumer(nullptr);
		}

		/// <summary>
		///		将同一个节目的各个流上的 PtsAdjustPipe 放到列表里，传进来，本函数将计算新的 pts 偏移量
		///		并帮它们设置新的偏移量。
		/// </summary>
		/// <param name="pts_adjust_pipes"></param>
		/// <returns></returns>
		static void CalculateNewPtsOffset(List<shared_ptr<PtsAdjustPipe>> pts_adjust_pipes)
		{
			// 求出各个管道的仅供参考的新的偏移量，选出其中的最大值。
			int64_t max_pts_offset = 0;
			for (shared_ptr<PtsAdjustPipe> &pipe : pts_adjust_pipes)
			{
				int64_t ref_offset = pipe->_pts_offset + pipe->_last_frame_pts + pipe->_last_two_frame_pts_interval;
				if (ref_offset > max_pts_offset)
				{
					max_pts_offset = ref_offset;
				}
			}

			// 对列表中的每一项应用最大的偏移量。
			for (shared_ptr<PtsAdjustPipe> &pipe : pts_adjust_pipes)
			{
				pipe->_pts_offset = max_pts_offset;
			}
		}
	};
}