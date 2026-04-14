#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <string>
#include <thread>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

int main(int argc, char* argv[])
{
    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(std::atoi("8080"));
    net::io_context ioc{1};
    tcp::acceptor acceptor{ioc, {address, port}};
    for(;;)
    {
	tcp::socket socket{ioc};
	acceptor.accept(socket);
	std::cout<< "Socket accepted" << std::endl;
	std::thread{std::bind([q=std::move(socket)]() mutable{
	    websocket::stream<tcp::socket> ws{std::move(const_cast<tcp::socket&>(q))};

            ws.set_option(websocket::stream_base::decorator(
                [](websocket::response_type& res)
                {
                    res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-sync");
            }));

	    ws.accept();
	    while(true)
            {
                try
                {
		    beast::flat_buffer buffer;
		    ws.read(buffer);
		    auto out = beast::buffers_to_string(buffer.cdata());
		    std::cout << out << std::endl;
		    ws.write(buffer.data());
		}
		catch(beast::system_error const& se)
		{
		    if(se.code() != websocket::error::closed)
            	    {
                	std::cerr << "Error: " << se.code().message() << std::endl;
                        break;
                    }
		}
	    }
	})}.detach();
    }
    return 0;
}