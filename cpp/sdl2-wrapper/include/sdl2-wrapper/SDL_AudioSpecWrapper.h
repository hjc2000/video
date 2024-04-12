#pragma once
#include<SDL.h>
#include<string>
#include<sstream>
#include<jccpp/Wrapper.h>
#include<ffmpeg-wrapper/info-collection/IAudioFrameInfoCollection.h>
#include<jccpp/Json.h>

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

	public:
		SDL_AudioSpecWrapper()
		{
			_wrapped_obj = &_spec;
		}

		SDL_AudioSpecWrapper(IAudioFrameInfoCollection &infos) :SDL_AudioSpecWrapper()
		{
			set_sample_format(infos.sample_format());
			set_sample_rate(infos.sample_rate());
			set_nb_samples(infos.nb_samples());
			set_ch_layout(infos.ch_layout());
		}

		SDL_AudioSpecWrapper(IAudioStreamInfoCollection &infos) :SDL_AudioSpecWrapper()
		{
			set_sample_format(infos.sample_format());
			set_sample_rate(infos.sample_rate());
			set_ch_layout(infos.ch_layout());
		}

		SDL_AudioSpecWrapper(SDL_AudioSpecWrapper &other) :SDL_AudioSpecWrapper()
		{
			_spec = other._spec;
		}

		void operator=(SDL_AudioSpecWrapper &other)
		{
			_spec = other._spec;
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
		AVRational TimeBase() override;
		void set_time_base(AVRational value) override;

		/// <summary>
		///		获取采样格式
		/// </summary>
		/// <returns>
		///		如果 SDL_AudioSpec 的 format 字段内储存的 SDL 采样格式没有对应的 AVSampleFormat 值，
		///		则会返回 AVSampleFormat::AV_SAMPLE_FMT_NONE
		/// </returns>
		AVSampleFormat sample_format() override;

		/// <summary>
		///		设置采样格式
		/// </summary>
		/// <param name="value">
		///		如果设置的 AVSampleFormat 值没有对应的 SDL 值，则会将 SDL_AudioSpec 的 format 字段
		///		设置为 0.
		/// </param>
		void set_sample_format(AVSampleFormat value) override;

		int sample_rate() override;
		void set_sample_rate(int value) override;

		AVChannelLayout ch_layout() override;
		void set_ch_layout(AVChannelLayout value) override;

		int nb_samples() override;
		void set_nb_samples(int value) override;
	};
}