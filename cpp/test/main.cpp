//#include<InfiniteVideoStream.h>
//#include<test_tsduck.h>
//
//using namespace video;

//int main(void)
//{
//	try
//	{
//		std::filesystem::current_path(Predefine_ResourceDir);
//		//test_ProgramEncoderMuxer();
//		//test_AVPacketPlayer();
//		test_tsduck();
//		return 0;
//	}
//	catch (jc::Exception &e)
//	{
//		cout << e.what() << endl;
//		throw;
//	}
//}

#include<iostream>
#include<test_boost_http.h>

int main()
{
	try
	{
		boost_send_http_request_to_baidu();
	}
	catch (std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}