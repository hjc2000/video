#include"test_boost_asio.h"
#include<boost/asio.hpp>
#include<boost/asio/connect.hpp>
#include<boost/asio/ip/tcp.hpp>
#include<boost/beast/core.hpp>
#include<boost/beast/http.hpp>
#include<boost/beast/version.hpp>
#include<iostream>
#include<string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using boost::asio::ip::tcp;
using boost::asio::ip::udp;

void boost_send_http_request_to_baidu()
{
	// 创建一个io_context对象
	boost::asio::io_context io_context;

	// 解析域名获取TCP终端
	tcp::resolver resolver(io_context);
	auto endpoints = resolver.resolve("www.baidu.com", "80");

	// 创建一个socket
	tcp::socket socket(io_context);

	// 连接到服务器
	boost::asio::connect(socket, endpoints);

	// 构造HTTP请求消息
	std::string request = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n";

	// 发送HTTP请求
	boost::asio::write(socket, boost::asio::buffer(request));

	// 读取响应
	boost::asio::streambuf response;
	boost::asio::read_until(socket, response, "\r\n\r\n");

	// 将响应头打印到标准输出
	std::istream response_stream(&response);
	std::string header;
	while (std::getline(response_stream, header) && header != "\r")
	{
		std::cout << header << "\n";
	}

	std::cout << "\n";

	// 如果还有剩余的消息在streambuf中，则继续打印
	if (response.size() > 0)
	{
		std::cout << &response;
	}

	// 继续读取直到EOF，打印所有读取的内容
	boost::system::error_code error;
	while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
	{
		std::cout << &response;
	}

	if (error != boost::asio::error::eof)
	{
		throw boost::system::system_error(error);
	}
}

void test_beast_http_request()
{
	// Set up an I/O context and a resolver
	net::io_context ioc;
	tcp::resolver resolver{ ioc };

	// Establish a connection to the server
	auto const results = resolver.resolve("www.baidu.com", "80");
	tcp::socket socket{ ioc };
	net::connect(socket, results.begin(), results.end());

	// Set up the request
	http::request<http::string_body> req{ http::verb::get, "/", 11 };
	req.set(http::field::host, "www.baidu.com");
	req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

	// Send the HTTP request
	http::write(socket, req);

	// Prepare the buffer for the response
	beast::flat_buffer buffer;

	// Receive the HTTP response
	http::response<http::dynamic_body> res;
	http::read(socket, buffer, res);

	// Write the message to standard out
	std::cout << res << std::endl;

	// Gracefully close the socket
	beast::error_code ec;
	socket.shutdown(tcp::socket::shutdown_both, ec);

	// Not_connected happens sometimes so don't throw if it occurs.
	if (ec && ec != beast::errc::not_connected)
	{
		throw beast::system_error{ ec };
	}

	// If we get here then the connection is closed gracefully
}

void receive_udp_message()
{
	boost::asio::io_service io_service;

	// 创建一个UDP socket来监听9999端口
	udp::socket socket(io_service, udp::endpoint(udp::v4(), 9999));
	//udp::socket socket{
	//	io_service,
	//	udp::endpoint{
	//		boost::asio::ip::address::from_string("192.168.8.5"),
	//		9999
	//	}
	//};

	while (true)
	{
		// 无限循环，持续监听
		char recv_buf[1024];
		udp::endpoint remote_endpoint;
		boost::system::error_code error;

		// 阻塞直到接收到消息
		size_t len = socket.receive_from(
			boost::asio::buffer(recv_buf),
			remote_endpoint,
			0,
			error
		);

		if (error && error != boost::asio::error::message_size)
		{
			throw boost::system::system_error(error);
		}

		std::cout << "Received: ";
		std::cout.write(recv_buf, len);
		std::cout << std::endl;
	}
}

void send_udp_message()
{
	try
	{
		boost::asio::io_service io_service;

		// 创建UDP socket
		udp::socket socket(io_service);

		// 打开socket为IPv4通信
		socket.open(udp::v4());

		// 目标端点，替换成你的目标IP地址和端口
		udp::endpoint target_endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 9999);

		// 消息内容，替换成你想发送的消息
		std::string message = "Hello UDP";

		while (1)
		{
			// 发送数据
			socket.send_to(boost::asio::buffer(message), target_endpoint);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		// 关闭socket（可选）
		socket.close();

		std::cout << "Message sent: " << message << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}
