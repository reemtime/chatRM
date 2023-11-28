#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

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


using boost::asio::ip::tcp;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    // Create a socket in the heap
    static pointer create(boost::asio::io_context& io_context, db_server& db_server);

    tcp::socket& socket();

    // Function for using a socket

    void start();

private:
    tcp_connection(boost::asio::io_context& io_context, db_server& db_server);

    void clear_message();

    void handle_message_read(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_data(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_auth(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_register(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_menu(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_create_room(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_delete_room(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_room_join(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_room_message(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_room_kick_member(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_room_add_member(const boost::system::error_code& error, size_t bytes_transferred);

    tcp::socket socket_;
    std::string username_;
    std::string room_;
    db_server& db_server_;
    boost::array<char, 2048> buf_;
    boost::serialization::Message client_msg_;
    boost::serialization::Message server_msg_;
};

#endif // TCP_CONNECTION_HPP
