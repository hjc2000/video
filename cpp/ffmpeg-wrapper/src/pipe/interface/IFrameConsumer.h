#pragma once
#include<AVFrameWrapper.h>

namespace video
{
	/**
	 * @brief 送入帧失败。
	*/
	class SendFrameException :public jc::Exception
	{
	public:
		SendFrameException() :Exception(__func__) {}
		SendFrameException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		SendFrameException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	/**
	 * @brief 帧消费者
	*/
	class IFrameConsumer
	{
	public:
		virtual ~IFrameConsumer() {}

	public:
		/**
		 * @brief 送入帧
		 * @param frame 要送入的帧。送入空指针表示冲洗。
		 * @exception SendFrameException 送入帧失败会抛出异常
		*/
		virtual void SendFrame(AVFrameWrapper *frame) = 0;
	};
}