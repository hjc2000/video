#pragma once
#include<include_ffmpeg.h>
#include<ErrorCode.h>
#include<PeriodicSignalSource.h>
#include<ISignalSource.h>

namespace video
{
	/**
	 * @brief 随机信号
	*/
	class RandomSignalSource :public ISignalSource
	{
	public:
		RandomSignalSource()
		{

		}

		// 通过 ISignalSource 继承
		double sample() override;
	};
}