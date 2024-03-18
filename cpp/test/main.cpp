#include<InfiniteVideoStream.h>
#include<test_tsduck.h>

using namespace video;

int main(void)
{
	std::filesystem::current_path(Predefine_ResourceDir);
	test_ProgramEncoderMuxer();
	//test_AVPacketPlayer();
	//test_tsduck();
	return 0;
}