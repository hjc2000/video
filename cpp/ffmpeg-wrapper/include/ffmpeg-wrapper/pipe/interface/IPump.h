#pragma once
#include<jccpp/CancellationTokenSource.h>

namespace video
{
	class IPump
	{
	public:
		/// <summary>
		///		本类作为中转站。将数据从源拉过来送给下级的消费者。
		/// </summary>
		/// <param name="cancel_pump"></param>
		virtual void Pump(shared_ptr<CancellationToken> cancel_pump) = 0;
	};
}