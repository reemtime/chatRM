#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <iostream>
#include <vector>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <sstream>

#include "db.h"
#include "message.hpp"
#include "tcp_connection.hpp"

using boost::asio::ip::tcp;

class tcp_server
{
public:
    tcp_server(boost::asio::io_context& io_context);

private:
    // Listen and accept new connections
    void start_connect();

    void handle_accept(tcp_connection::pointer new_connection,
        const boost::system::error_code& e);

private:
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    db_server db_server_;
};


#endif // TCP_SERVER_HPP
