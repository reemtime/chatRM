#include "tcp_connection.hpp"

using boost::asio::ip::tcp;

typedef boost::shared_ptr<tcp_connection> pointer;

// Create a socket in the heap
pointer tcp_connection::create(boost::asio::io_context& io_context, db_server& db_server)
{
    return pointer(new tcp_connection(io_context, db_server));
}

tcp::socket& tcp_connection::socket()
{
    return socket_;
}

// Function for using a socket

void tcp_connection::start()
{
    try
    {
    std::cout << "Client connected!" << std::endl;
    socket_.async_read_some(boost::asio::buffer(buf_),
        bind(&tcp_connection::handle_data, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    catch (const sql::SQLException& e)
    {
        std::string response = "Database error";
        std::cerr << "Database error: " << e.what() << '\n';

        socket_.async_write_some(boost::asio::buffer(response),
            bind(&tcp_connection::handle_data, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        start();
    }
}

tcp_connection::tcp_connection(boost::asio::io_context& io_context, db_server& db_server)
    : socket_(io_context), db_server_(db_server)
{
    void clear_message();
}

void tcp_connection::clear_message()
{
    server_msg_.status = 0;
    server_msg_.command = "";
    server_msg_.data.clear();

    client_msg_.status = 0;
    client_msg_.command = "";
    client_msg_.data.clear();
}

void tcp_connection::handle_message_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    socket_.async_read_some(boost::asio::buffer(buf_),
        bind(&tcp_connection::handle_data, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::handle_data(const boost::system::error_code& error, size_t bytes_transferred)
{
    try
    {
        clear_message();
        std::string message(buf_.data(), bytes_transferred);
        client_msg_ = boost::serialization::deserialize_message(message);

        if(client_msg_.status == 0)
        {
            std::cout << "Flag: " << client_msg_.command << std::endl;

            if (client_msg_.command == "auth")
            {
                handle_auth(error, bytes_transferred);
            }

            else if (client_msg_.command == "register")
            {
                handle_register(error, bytes_transferred);
            }

            else if (client_msg_.command == "/menu")
            {
                handle_menu(error, bytes_transferred);
            }

            else if (client_msg_.command == "/join")
            {
                handle_room_join(error, bytes_transferred);
            }

            else if (client_msg_.command == "/create")
            {
                handle_create_room(error, bytes_transferred);
            }

            else if (client_msg_.command == "/delete")
            {
                handle_delete_room(error, bytes_transferred);
            }
            else if(client_msg_.command == "/message")
            {
                handle_room_message(error, bytes_transferred);
            }
            else if(client_msg_.command == "/add")
            {
                handle_room_add_member(error, bytes_transferred);
            }
            else if(client_msg_.command == "/kick")
            {
                handle_room_kick_member(error, bytes_transferred);
            }
            else
            {
                handle_menu(error, bytes_transferred);
            }
        }
        else
        {
            std::cerr << "error: " << error.message() << std::endl;
            socket_.close();
        }
    }
    catch (const sql::SQLException& db_error)
    {
        std::cerr << "SQL error: " << db_error.what() << '\n';
        clear_message();
        server_msg_.status = 1;
        server_msg_.command = "error";
        server_msg_.data.push_back("sql error");

        socket_.async_write_some(boost::asio::buffer(boost::serialization::serialize_message(server_msg_)),
            bind(&tcp_connection::handle_message_read, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
    catch(const std::exception& error)
    {
        std::cerr << "error: " << error.what() << std::endl;
        socket_.close();
    }
}

void tcp_connection::handle_auth(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        username_ = client_msg_.data.at(0);
        std::string password = client_msg_.data.at(1);
        std::cout << username_ << std::endl;
        server_msg_.command = "auth";

        if (db_server_.authenticateUser(username_, password))
        {
            std::cout << "auth successed\n";
            server_msg_.data.push_back("ok");
            socket_.async_write_some(boost::asio::buffer(boost::serialization::serialize_message(server_msg_)),
                bind(&tcp_connection::handle_message_read, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            std::cout << "auth failed\n";
            server_msg_.data.push_back("error");
            socket_.async_write_some(boost::asio::buffer(boost::serialization::serialize_message(server_msg_)),
                bind(&tcp_connection::handle_message_read, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }
    else
    {
        std::cout << "Error auth" << std::endl;
    }
}

void tcp_connection::handle_menu(const boost::system::error_code& error, size_t bytes_transferred)
{
    server_msg_.command = "/menu";
    server_msg_.data = db_server_.showRooms(username_);

    if (server_msg_.data.empty())
    {
        server_msg_.data.push_back("You don't have a rooms.");
        socket_.async_write_some(boost::asio::buffer(boost::serialization::serialize_message(server_msg_)),
            bind(&tcp_connection::handle_message_read, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        socket_.async_write_some(boost::asio::buffer(boost::serialization::serialize_message(server_msg_)),
            bind(&tcp_connection::handle_message_read, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
}

void tcp_connection::handle_register(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        username_ = client_msg_.data.at(0);
        std::string password = client_msg_.data.at(1);

        server_msg_.command = "register";

        if (db_server_.registerUser(username_, password))
        {
            std::cout << username_ << " is registered\n";
            server_msg_.data.push_back("ok");
            socket_.async_write_some(boost::asio::buffer(boost::serialization::serialize_message(server_msg_)),
                bind(&tcp_connection::handle_message_read, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            std::cout << "register failed\n";
            server_msg_.data.push_back("error");
            socket_.async_write_some(boost::asio::buffer(boost::serialization::serialize_message(server_msg_)),
                bind(&tcp_connection::handle_message_read, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }
    else
    {
        std::cout << "Error register" << std::endl;
    }
}

void tcp_connection::handle_create_room(const boost::system::error_code& error, size_t bytes_transferred)
{
    if(!error)
    {
        room_ = client_msg_.data.at(0);

        db_server_.createRoom(room_);

        db_server_.addMember(room_, username_);

        std::cout << "Room has been added.\n";
        handle_menu(error, bytes_transferred);
    }
    else
    {
        std::cout << "Error create room" << std::endl;
    }
}

void tcp_connection::handle_delete_room(const boost::system::error_code& error, size_t bytes_transferred)
{
    if(!error)
    {
        room_ = client_msg_.data.at(0);
        db_server_.deleteRoom(room_);

        std::cout << "Room has been deleted.\n";
        handle_menu(error, bytes_transferred);
    }
    else
    {
        std::cout << "Error create room" << std::endl;
    }
}

void tcp_connection::handle_room_join(const boost::system::error_code& error, size_t bytes_transferred)
{
    room_ = client_msg_.data.at(0);
    server_msg_.command = "/join";
    server_msg_.data = db_server_.joinToRoom(room_);

    if (server_msg_.data.empty())
    {
        server_msg_.data.push_back("You don't have a message.");
        socket_.async_write_some(boost::asio::buffer(boost::serialization::serialize_message(server_msg_)),
            bind(&tcp_connection::handle_message_read, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        socket_.async_write_some(boost::asio::buffer(boost::serialization::serialize_message(server_msg_)),
            bind(&tcp_connection::handle_message_read, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
}

void tcp_connection::handle_room_message(const boost::system::error_code& error, size_t bytes_transferred)
{
    if(!error)
    {
        std::string message = client_msg_.data.at(0);

        db_server_.addMessage(username_, room_, message);

        client_msg_.data.back() = room_;
        handle_room_join(error, bytes_transferred);

    }
    else
    {

    }

}

void tcp_connection::handle_room_kick_member(const boost::system::error_code& error, size_t bytes_transferred)
{
    if(!error)
    {
        std::string user_name = client_msg_.data.at(0);

        if (db_server_.kickMember(room_, user_name))
        {
            std::string message = user_name + " has been deleted";
            db_server_.addMessage(username_, room_, message);
            client_msg_.data.back() = room_;
            handle_room_join(error, bytes_transferred);
        }
        else
        {
            client_msg_.data.back() = room_;
            handle_room_join(error, bytes_transferred);
        }
    }
    else
    {

    }
}

void tcp_connection::handle_room_add_member(const boost::system::error_code& error, size_t bytes_transferred)
{
    if(!error)
    {
        std::string user_name = client_msg_.data.at(0);

        if (db_server_.addMember(room_, user_name))
        {
            std::string message = user_name + " join to the room.";
            db_server_.addMessage(username_, room_, message);
            client_msg_.data.back() = room_;
            handle_room_join(error, bytes_transferred);
        }
        else
        {
            client_msg_.data.back() = room_;
            handle_room_join(error, bytes_transferred);
        }
    }
    else
    {

    }
}

