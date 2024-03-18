#pragma once
#include<AVCalculate.h>
#include<AVCompare.h>
#include<IPacketConsumer.h>
#include<PipePacketSource.h>
#include<av_std_out.h>

namespace video
{
	class InfinitePacketPipe :public IPacketConsumer, public PipePacketSource
	{
	private:
		#pragma region 校准起始时间戳
		bool _start_pts_dts_not_set = true;
		int64_t _start_pts = 0;
		int64_t _start_dts = 0;
		int64_t _correct_offset = 0;

		void CorrectStartTimeStamp(AVPacketWrapper &packet)
		{
			if (_start_pts_dts_not_set)
			{
				_start_pts_dts_not_set = false;
				_start_pts = packet.pts();
				_start_dts = packet.dts();

				// 开头处对齐取最小值，因为不能出现时间戳回溯。
				_correct_offset = std::min(_start_pts, _start_dts);
			}

			packet.set_pts(packet.pts() - _correct_offset);
			packet.set_dts(packet.dts() - _correct_offset);
		}
		#pragma endregion

		int64_t _last_pts = 0;
		int64_t _last_dts = 0;
		int64_t _offset = 0;
		int64_t _last_packet_duration = 0;

		void UpdateLastPacketDuration(int64_t value)
		{
			if (value > _last_packet_duration)
			{
				_last_packet_duration = value;
			}
		}

		void UpdateLastPts(int64_t value)
		{
			if (value > _last_pts)
			{
				_last_pts = value;
			}
		}

		void UpdateLastDts(int64_t value)
		{
			if (value > _last_dts)
			{
				_last_dts = value;
			}
		}

	public:
		/// <summary>
		///		送入包。会在调整包的 pts 和 dts 后送给消费者。如果送入空指针，会直接返回，不会将空指针送给消费者，
		///		所以本函数无法冲洗消费者。冲洗消费者需要调用 FlushConsumer 函数。
		/// 
		///		* 送进来的包的时间基必须是 1/90000，否则会抛出异常。
		/// </summary>
		/// <param name="packet"></param>
		void SendPacket(AVPacketWrapper *packet) override
		{
			if (!packet)
			{
				// 结尾处添加边距取最大值，不能让下一个封装的时间戳重叠到上一个。
				_offset += std::max(_last_pts, _last_dts) + _last_packet_duration;
				_last_pts = 0;
				_last_dts = 0;
				_last_packet_duration = 0;
				_start_pts_dts_not_set = true;
				return;
			}

			// packet 不为空指针
			if (packet->time_base() != AVRational{ 1,90000 })
			{
				cout << CODE_POS_STR << "送进来的包的时间基必须是 " << AVRational{ 1,90000 } << endl;
			}

			CorrectStartTimeStamp(*packet);
			UpdateLastPts(packet->pts());
			UpdateLastDts(packet->dts());
			UpdateLastPacketDuration(packet->Duration());
			packet->set_pts(packet->pts() + _offset);
			packet->set_dts(packet->dts() + _offset);
			SendPacketToEachConsumer(packet);
		}

		/// <summary>
		///		向 send_packet 送入空指针会直接返回，不会向消费者送入空指针从而冲洗消费者。
		///		只有调用本函数才会向消费者送入空指针，从而冲洗消费者。
		/// </summary>
		void FlushConsumer()
		{
			SendPacketToEachConsumer(nullptr);
		}
	};
}