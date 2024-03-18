#include<FileTsSource.h>
#include<chrono>

using namespace std;
using namespace std::chrono;

vk_result FileTsSource::cross_stream_file_get(const char *filename, shared_ptr<FileTsSource> pcstream)
{
	vk_result nres = vatek_memfail;

	if (pcstream)
	{
		pcstream->fhandle = fopen(filename, "rb");
		nres = vatek_format;
		if (pcstream->fhandle)
		{
			fseek(pcstream->fhandle, 0, SEEK_END);
			pcstream->file_size = (int32_t)ftell(pcstream->fhandle);
			fseek(pcstream->fhandle, 0, SEEK_SET);

			nres = pcstream->file_lock();
			if (!is_vatek_success(nres))
				fclose(pcstream->fhandle);
		}
	}

	return nres;
}

vk_result FileTsSource::Start()
{
	if (fhandle)
	{
		this_thread::sleep_for(milliseconds(1000));					//wait A3 chip ready
		tick = DateTime::GetSteadyTimeInMilliseconds();
		return vatek_success;
	}

	return vatek_badstatus;
}

uint32_t FileTsSource::Bitrate()
{
	int64_t eclipse = DateTime::GetSteadyTimeInMilliseconds() - tick;
	if (eclipse)
	{
		uint32_t bitrate = ((int64_t)_amout_of_total_packets_read * TS_PACKET_LEN * 8 * 1000) / eclipse;
		tick = DateTime::GetSteadyTimeInMilliseconds();
		_amout_of_total_packets_read = 0;
		return bitrate;
	}

	return 0;
}

vk_result FileTsSource::Stop()
{
	return vatek_success;
}

void FileTsSource::Close()
{
	Stop();
	if (fhandle)
	{
		fclose(fhandle);
		fhandle = NULL;
	}
}
