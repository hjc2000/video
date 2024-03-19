#include"test_boost_http.h"
#include<boost/asio.hpp>
#include<iostream>
#include<string>

using boost::asio::ip::tcp;

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
