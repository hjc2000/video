#include"JoinedInputFormatDemuxDecoder.h"

using namespace video;
using namespace std;

void video::JoinedInputFormatDemuxDecoder::OpenInputIfNull()
{
	if (_current_intput_format == nullptr && _get_format_callback != nullptr)
	{
		_current_intput_format = _get_format_callback();
	}
}
