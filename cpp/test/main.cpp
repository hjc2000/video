﻿#include<QtCore/QCoreApplication>
#include<QtCore/QDebug>
#include<ffmpeg-wrapper/mux/SptsEncodeMux.h>
#include<filesystem>
#include<sdl2-wrapper/AVPacketPlayer.h>
#include<test_tsduck.h>

using namespace video;

int main(void)
{
	try
	{
		std::filesystem::current_path(Predefine_ResourceDir);
		//test_SptsEncodeMux();
		//test_AVPacketPlayer();
		test_tsduck();
		return 0;
	}
	catch (jc::Exception &e)
	{
		cout << e.what() << endl;
		throw;
	}
}

//int main()
//{
//	int argc = 1;
//	char *argv[] = {(char*)"appname", nullptr};
//	QCoreApplication app(argc, argv);
//
//	qDebug() << "Hello, Qt console application!";
//
//	return app.exec();
//}

//#include<iostream>
//#include<test_boost_asio.h>
//#include<thread>
//
//int main()
//{
//	try
//	{
//		std::thread([&]()
//		{
//			try
//			{
//				receive_udp_message();
//			}
//			catch (std::exception &e)
//			{
//				std::cerr << "Exception: " << e.what() << "\n";
//			}
//		}).detach();
//
//		std::thread([&]()
//		{
//			try
//			{
//				send_udp_message();
//			}
//			catch (std::exception &e)
//			{
//				std::cerr << "Exception: " << e.what() << "\n";
//			}
//		}).detach();
//
//		std::cin.get();
//	}
//	catch (std::exception &e)
//	{
//		std::cerr << "Exception: " << e.what() << "\n";
//	}
//
//	return 0;
//}
