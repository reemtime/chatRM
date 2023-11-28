#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "message.hpp"

class Client
{
    public:

        Client(boost::asio::io_context& io_context,const boost::asio::ip::tcp::resolver::results_type& endpoints);
        ~Client();
        void close();

    private:
        void connect(const boost::asio::ip::tcp::resolver::results_type& endpoints);

        void user_input();

        void room_message();

        void send_message();

        void receive_message();

        void clear_screen();

        void clear_message();

        void authentication();

        void registration();

        void error_message(std::string& error);

        void help();

    private:
        boost::asio::io_context& io_context_;
        boost::asio::ip::tcp::socket socket_;
        boost::array<char, 1024> buf_;
        boost::serialization::Message client_msg_;
        boost::serialization::Message server_msg_;
        std::string error_;
        std::string room_;
};

#endif