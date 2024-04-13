#pragma once
#include<SDL.h>
#include<ffmpeg-wrapper/info-collection/IAudioFrameInfoCollection.h>
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
		SDL_AudioSpec _spec = SDL_AudioSpec{};
		SDL_AudioSpec *_wrapped_obj = nullptr;

	public:
		SDL_AudioSpecWrapper()
		{
			_wrapped_obj = &_spec;
		}

		SDL_AudioSpecWrapper(IAudioFrameInfoCollection &infos)
		{
			_wrapped_obj = &_spec;
			SetSampleFormat(infos.SampleFormat());
			SetSampleRate(infos.SampleRate());
			SetSampleCount(infos.SampleCount());
			SetChannelLayout(infos.ChannelLayout());
		}

		SDL_AudioSpecWrapper(IAudioStreamInfoCollection &infos)
		{
			_wrapped_obj = &_spec;
			SetSampleFormat(infos.SampleFormat());
			SetSampleRate(infos.SampleRate());
			SetChannelLayout(infos.ChannelLayout());
		}

		SDL_AudioSpecWrapper(SDL_AudioSpecWrapper &other)
		{
			*this = other;
		}

		void operator=(SDL_AudioSpecWrapper &other)
		{
			_spec = other._spec;
			_wrapped_obj = &_spec;
		}

		SDL_AudioSpec *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		SDL_AudioSpec *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		bool operator==(SDL_AudioSpecWrapper &other)
		{
			return _wrapped_obj->freq == other->freq &&
				_wrapped_obj->format == other->format &&
				_wrapped_obj->channels == other->channels &&
				_wrapped_obj->samples == other->samples &&
				_wrapped_obj->size == other->size;
		}

		Json ToJson() override
		{
			return Json{
				{"freq",_wrapped_obj->freq},
				{
					"format",
					{
						{"SDL_AudioFormat",_wrapped_obj->format},
						{"AVSampleFormat",sample_format_string()},
					},
				},
				{"channels",(int)_wrapped_obj->channels},
				{
					// 这个值表示：什么值表示静音。对于 s8，静音值是 0，对于 u8，静音值是 128.
					"silence",
					(int)_wrapped_obj->silence
				},
				{"samples",_wrapped_obj->samples},
				{"size",_wrapped_obj->size},
			};

			// 这个值是 SDL 内部使用的，用户不能使用，作用是内存对齐
			//sb << "padding = " << _wrapped_obj->padding << endl;
		}

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