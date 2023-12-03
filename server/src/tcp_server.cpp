#include "tcp_server.hpp"

tcp_server::tcp_server(boost::asio::io_context& io_context)
    : io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), 44444))
{
    start_connect();
}

// Listen and accept new connections
void tcp_server::start_connect()
{
    tcp_connection::pointer new_connection =
        tcp_connection::create(io_context_, db_server_);

    acceptor_.async_accept(new_connection->socket(),
        boost::bind(&tcp_server::handle_accept, this, new_connection,
            boost::asio::placeholders::error));
}

void tcp_server::handle_accept(tcp_connection::pointer new_connection,
    const boost::system::error_code& e)
{
    if (!e)
    {
        new_connection->start();
    }
    start_connect();
}

