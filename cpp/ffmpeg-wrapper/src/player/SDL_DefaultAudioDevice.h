#pragma once
#include<IAudioFrameInfoCollection.h>
#include<IDisposable.h>
#include<Json.h>
#include<SDL_AudioSpecWrapper.h>
#include<SDL_Util.h>
#include<TaskCompletionSignal.h>
#include<atomic>
#include<mutex>
#include<semaphore>
#include<sstream>

namespace video
{
	using namespace std;

	/**
	 * @brief SDL 默认的音频设备。
	 * - 本类线程安全。
	 * - 本类作为 IAudioFrameInfoCollection 派生类，提供的是音频驱动器的信息，而不是构造函数传进来的
	 *   输入音频流的信息。
	*/
	class SDL_DefaultAudioDevice :
		public IAudioFrameInfoCollection,
		public IJsonSerializable,
		public IDisposable
	{
	public:
		/// <summary>
		///		构造函数。会调用 SDL 打开音频设备。打开失败会抛出异常。
		/// </summary>
		SDL_DefaultAudioDevice()
		{
			_desired_spec->callback = static_audio_callback;
			_desired_spec->userdata = this;

			int device_id = SDL_OpenAudio(_desired_spec, _abtained_spec);
			if (device_id < 0)
			{
				string error_str = "Could not open audio: " + string(SDL_GetError());
				throw jc::Exception(error_str);
			}
		}

		~SDL_DefaultAudioDevice()
		{
			Dispose();
			cout << "~SDL_DefaultAudioDevice()" << endl;
		}

		/// <summary>
		///		* 本方法会阻塞，直到 _audio_callback 返回。
		/// </summary>
		void Dispose()
		{
			std::lock_guard l(_not_private_methods_lock);
			if (_disposed) return;
			_disposed = true;

			SDL_CloseAudio();
			cout << "SDL_DefaultAudioDevice 释放" << endl;
		}

	private:
		atomic_bool _disposed = false;
		SDL_AudioSpecWrapper _desired_spec;
		SDL_AudioSpecWrapper _abtained_spec;

		static void static_audio_callback(void *userdata, uint8_t *stream, int len)
		{
			SDL_DefaultAudioDevice *me = (SDL_DefaultAudioDevice *)userdata;
			me->_audio_callback(stream, len);
		}

		std::mutex _not_private_methods_lock;

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
		void Pause(bool pause)
		{
			std::lock_guard l(_not_private_methods_lock);
			SDL_PauseAudio(pause);
		}

		#pragma region IAudioStreamInfoCollection
		/* 这里只有 get 访问器有效，set 访问器什么都不做。所以这里的方法都不用加锁。*/

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

		Json ToJson() override
		{
			return Json{
				{"_desired_spec",_desired_spec.ToJson()},
				{"_abtained_spec",_abtained_spec.ToJson()},
			};
		}
	};
}