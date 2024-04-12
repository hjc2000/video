#pragma once
#include<ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>

namespace video
{
	class AudioStreamInfoCollection :
		public IAudioStreamInfoCollection
	{
		#pragma region 生命周期
	public:
		AudioStreamInfoCollection() {}

		AudioStreamInfoCollection(shared_ptr<IAudioStreamInfoCollection> value)
		{
			*this = *value;
		}

		AudioStreamInfoCollection(IAudioStreamInfoCollection const &value)
		{
			*this = value;
		}

		AudioStreamInfoCollection &operator=(IAudioStreamInfoCollection &value)
		{
			IAudioStreamInfoCollection::operator=(value);
			return *this;
		}
		AudioStreamInfoCollection &operator=(IAudioStreamInfoCollection const &value)
		{
			IAudioStreamInfoCollection::operator=(value);
			return *this;
		}
		#pragma endregion

	public:
		AVRational _time_base{};
		AVSampleFormat _sample_format{};
		int _sample_rate = 0;
		AVChannelLayout _ch_layout{};

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

		int SampleRate() const override
		{
			return _sample_rate;
		}

		void SetSampleRate(int value) override
		{
			_sample_rate = value;
		}

		AVChannelLayout ChannelLayout() const override
		{
			return _ch_layout;
		}

		void SetChannelLayout(AVChannelLayout value) override
		{
			_ch_layout = value;
		}
	};

}