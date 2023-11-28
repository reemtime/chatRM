#include <iostream>

#include "db.h"
#include "message.hpp"
#include <thread>

#include "tcp_connection.hpp"
#include "tcp_server.hpp"

int main()
{
	try
	{
		boost::asio::io_context io_context;
		tcp_server server(io_context);
		io_context.run();

	}
    catch (std::exception& error)
	{
        std::cerr << error.what() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}
