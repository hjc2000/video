#include"test_beast_http_request.h"
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <iostream>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = net::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

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
