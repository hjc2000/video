#pragma once
#include<ffmpeg-wrapper/info-collection/AudioStreamInfoCollection.h>

namespace video
{
	class EncoderPipeFactory
	{
	public:
		#pragma region 单例
		static EncoderPipeFactory &Instance();

		/// <summary>
		///		派生此类，将实例赋值给本字段，可以让 Instance 方法返回自定义的工厂。
		/// </summary>
		static shared_ptr<EncoderPipeFactory> _custom_factory;
		#pragma endregion

	};
}