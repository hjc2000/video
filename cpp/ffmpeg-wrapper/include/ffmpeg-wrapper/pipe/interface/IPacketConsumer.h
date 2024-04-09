#pragma once
#include<ffmpeg-wrapper/wrapper/AVPacketWrapper.h>

namespace video
{
	class SendPacketException :public jc::Exception
	{
	public:
		SendPacketException() :Exception(__func__) {}
		SendPacketException(const char *msg) :Exception(std::string(__func__) + " " + msg) {}
		SendPacketException(std::string msg) :Exception(std::string(__func__) + " " + msg) {}
	};

	class IPacketConsumer
	{
	public:
		virtual ~IPacketConsumer() {}

	public:
		/**
		 * @brief
		 * @param packet
		 * @exception SendPacketException
		*/
		virtual void SendPacket(AVPacketWrapper *packet) = 0;
	};
}