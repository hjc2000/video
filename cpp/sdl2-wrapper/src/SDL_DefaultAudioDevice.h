#pragma once
#include<IAudioFrameInfoCollection.h>
#include<IDisposable.h>
#include<Json.h>
#include<SDL_AudioSpecWrapper.h>
#include<SDL_Initializer.h>
#include<TaskCompletionSignal.h>
#include<atomic>
#include<mutex>
#include<semaphore>
#include<sstream>

namespace video
{
	/// <summary>
	///		SDL 默认的音频设备。
	///		- 本类线程安全。
	///		- 本类作为 IAudioFrameInfoCollection 派生类，提供的是音频驱动器的信息，
	///		  而不是构造函数传进来的输入音频流的信息。
	/// </summary>
	class SDL_DefaultAudioDevice :
		public IAudioFrameInfoCollection,
		public IJsonSerializable,
		public IDisposable
	{
	public:
		/// <summary>
		///		构造函数。会调用 SDL 打开音频设备。打开失败会抛出异常。
		/// </summary>
		SDL_DefaultAudioDevice();
		~SDL_DefaultAudioDevice();

		/// <summary>
		///		本方法会阻塞，直到 _audio_callback 返回，所以不要在 _audio_callback
		///		里面调用本方法。
		/// </summary>
		void Dispose() override;

	private:
		std::atomic_bool _disposed = false;
		SDL_AudioSpecWrapper _desired_spec;
		SDL_AudioSpecWrapper _abtained_spec;
		std::mutex _not_private_methods_lock;

		static void static_audio_callback(void *userdata, uint8_t *stream, int len);

	public:
		/// <summary>
		///		音频回调委托。
		///		本委托函数一旦返回，就认为成功向 stream 中写入音频数据了。
		///		所以，一定要保证写入成功。
		/// </summary>
		/// <param name="stream">SDL 的音频缓冲区，需要将音频数据复制到这里。</param>
		/// <param name="len">音频缓冲区的大小。写入音频数据不可以超过此大小。</param>
		function<void(uint8_t *stream, int len)> _audio_callback;

		/// <summary>
		///		暂停或播放。
		///		* 本方法不会阻塞，可以在回调函数中使用。
		/// </summary>
		/// <param name="pause">传入 true 暂停，传入 false 播放</param>
		void Pause(bool pause);

		#pragma region IAudioStreamInfoCollection
		AVRational time_base() override;
		void set_time_base(AVRational value) override;
		AVSampleFormat sample_format() override;
		void set_sample_format(AVSampleFormat value) override;
		int sample_rate() override;
		void set_sample_rate(int value) override;
		AVChannelLayout ch_layout() override;
		void set_ch_layout(AVChannelLayout value) override;
		int nb_samples() override;
		void set_nb_samples(int value) override;
		#pragma endregion

		Json ToJson() override;
	};
}