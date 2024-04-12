#pragma once
#include<ffmpeg-wrapper/info-collection/IAudioFrameInfoCollection.h>

namespace video
{
	/// <summary>
	///		简单的储存音频信息的结构体
	/// </summary>
	class AudioFrameInfoCollection :public IAudioFrameInfoCollection
	{
		#pragma region 生命周期
	public:
		AudioFrameInfoCollection() {}

		AudioFrameInfoCollection(IAudioStreamInfoCollection &infos, int nb_samples)
		{
			IAudioStreamInfoCollection::operator=(infos);
			_nb_samples = nb_samples;
		}

		AudioFrameInfoCollection(IAudioFrameInfoCollection &infos)
		{
			IAudioFrameInfoCollection::operator=(infos);
		}

		AudioFrameInfoCollection &operator=(IAudioFrameInfoCollection &value)
		{
			IAudioFrameInfoCollection::operator=(value);
			return *this;
		}
		#pragma endregion

	public:
		AVRational _time_base{};
		AVSampleFormat _sample_format{};
		int _sample_rate = 0;
		AVChannelLayout _ch_layout{};
		int _nb_samples = 0;

	public:
		AVRational TimeBase() const override
		{
			return _time_base;
		}

		void SetTimeBase(AVRational value) override
		{
			_time_base = value;
		}

		AVSampleFormat sample_format() const override
		{
			return _sample_format;
		}

		void set_sample_format(AVSampleFormat value) override
		{
			_sample_format = value;
		}

		int sample_rate() const override
		{
			return _sample_rate;
		}

		void set_sample_rate(int value) override
		{
			_sample_rate = value;
		}

		AVChannelLayout ch_layout() override
		{
			return _ch_layout;
		}

		void set_ch_layout(AVChannelLayout value) override
		{
			_ch_layout = value;
		}

		int nb_samples() override
		{
			return _nb_samples;
		}

		void set_nb_samples(int value) override
		{
			_nb_samples = value;
		}
	};
}