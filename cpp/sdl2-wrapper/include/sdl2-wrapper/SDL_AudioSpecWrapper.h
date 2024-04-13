#pragma once
#include<SDL.h>
#include<ffmpeg-wrapper/info-collection/IAudioFrameInfoCollection.h>
#include<jccpp/Exception.h>
#include<jccpp/Json.h>
#include<jccpp/Wrapper.h>
#include<sstream>
#include<string>

using std::string;
using std::stringstream;
using std::endl;
using namespace video;

namespace video
{
	class SDL_AudioSpecWrapper :
		public Wrapper<SDL_AudioSpec>,
		public IAudioFrameInfoCollection,
		public IJsonSerializable
	{
		SDL_AudioSpec _spec{};
		SDL_AudioSpec *_wrapped_obj = &_spec;

	public:
		SDL_AudioSpecWrapper() = default;

		SDL_AudioSpecWrapper(IAudioFrameInfoCollection &infos)
		{
			SetSampleFormat(infos.SampleFormat());
			SetSampleRate(infos.SampleRate());
			SetSampleCount(infos.SampleCount());
			SetChannelLayout(infos.ChannelLayout());
		}

		SDL_AudioSpecWrapper(IAudioStreamInfoCollection &infos)
		{
			SetSampleFormat(infos.SampleFormat());
			SetSampleRate(infos.SampleRate());
			SetChannelLayout(infos.ChannelLayout());
		}

		SDL_AudioSpecWrapper(SDL_AudioSpecWrapper &another)
		{
			*this = another;
		}

		void operator=(SDL_AudioSpecWrapper &another)
		{
			_spec = another._spec;
		}

		SDL_AudioSpec *&WrappedObj() override
		{
			throw jc::NotSupportedException();
		}
		SDL_AudioSpec *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		bool operator==(SDL_AudioSpecWrapper &another)
		{
			return _wrapped_obj->freq == another->freq &&
				_wrapped_obj->format == another->format &&
				_wrapped_obj->channels == another->channels &&
				_wrapped_obj->samples == another->samples &&
				_wrapped_obj->size == another->size;
		}

		Json ToJson() override;

		// 通过 IAudioFrameInfoCollection 继承
		AVRational TimeBase() const override;
		void SetTimeBase(AVRational value) override;

		/// <summary>
		///		获取采样格式
		/// </summary>
		/// <returns>
		///		如果 SDL_AudioSpec 的 format 字段内储存的 SDL 采样格式没有对应的 AVSampleFormat 值，
		///		则会返回 AVSampleFormat::AV_SAMPLE_FMT_NONE
		/// </returns>
		AVSampleFormat SampleFormat() const override;

		/// <summary>
		///		设置采样格式
		/// </summary>
		/// <param name="value">
		///		如果设置的 AVSampleFormat 值没有对应的 SDL 值，则会将 SDL_AudioSpec 的 format 字段
		///		设置为 0.
		/// </param>
		void SetSampleFormat(AVSampleFormat value) override;

		int SampleRate() const override;
		void SetSampleRate(int value) override;

		AVChannelLayout ChannelLayout() const override;
		void SetChannelLayout(AVChannelLayout value) override;

		int SampleCount() const override;
		void SetSampleCount(int value) override;
	};
}