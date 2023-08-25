#include "config.h"
#include "db.h"

#include <iostream>
#include <vector>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <sstream>



using boost::asio::ip::tcp;

std::string serializerVector(const std::vector<std::string>& vector)
{
	std::ostringstream oss;
	for (auto elem : vector)
	{
		oss << elem << '\n';
	}

	return oss.str();
}

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	// Create a socket in the heap
	static pointer create(boost::asio::io_context& io_context, db_server& db_server)
	{
		return pointer(new tcp_connection(io_context, db_server));
	}


	tcp::socket& socket()
	{
		return socket_;
	}

	// Function for using a socket

	void start()
	{
		std::cout << "Client connected!" << std::endl;
		socket_.async_read_some(boost::asio::buffer(buf_),
			bind(&tcp_connection::handle_data, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
private:
	tcp_connection(boost::asio::io_context& io_context, db_server& db_server)
		: socket_(io_context), db_server_(db_server)
	{}

	void handle_flag_read(const boost::system::error_code& error, size_t bytes_transferred)
	{
		socket_.async_read_some(boost::asio::buffer(buf_),
			bind(&tcp_connection::handle_data, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_data(const boost::system::error_code& error, size_t bytes_transferred)
	{
		std::string flag(buf_.data(), bytes_transferred);
		std::cout << "Flag: " << flag << std::endl;

		if (flag == "auth")
		{
			socket_.async_read_some(boost::asio::buffer(buf_),
				bind(&tcp_connection::handle_auth, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		else if (flag == "register")
		{
			socket_.async_read_some(boost::asio::buffer(buf_),
				bind(&tcp_connection::handle_register, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		else if (flag == "show")
		{
			std::vector<std::string> rooms{};
			rooms = db_server_.showRooms(username_);
			std::string rooms_str;
			rooms_str = serializerVector(rooms);

			if (rooms.empty())
			{
				std::string response = "You don't have a rooms.\n";
				socket_.async_write_some(boost::asio::buffer(response),
					bind(&tcp_connection::handle_flag_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
			else
			{
				socket_.async_write_some(boost::asio::buffer(rooms_str),
					bind(&tcp_connection::handle_flag_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
			
		}

		else if (flag == "join")
		{
			// Receive room's name
			std::cout << "JOIN ROOM" << std::endl;
			socket_.async_read_some(boost::asio::buffer(buf_),
				bind(&tcp_connection::handle_room_join, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		else if (flag == "create")
		{
			socket_.async_read_some(boost::asio::buffer(buf_),
				bind(&tcp_connection::handle_create_room, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		else if (flag == "delete")
		{
			socket_.async_read_some(boost::asio::buffer(buf_),
				bind(&tcp_connection::handle_delete_room, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		else
		{
			std::cout << "ELSE" << std::endl;
			socket_.async_read_some(boost::asio::buffer(buf_),
				bind(&tcp_connection::handle_data, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
	}

	void handle_auth(const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (!error)
		{
			std::string result;
			std::string data(buf_.data(), bytes_transferred);
			size_t separator = data.find(':');
		

			username_ = data.substr(0, separator);
			std::string password = data.substr(separator + 1);

			std::cout << "User: " << username_ << std::endl;
			std::cout << "Pass: " << password << std::endl;

			if (db_server_.authenticateUser(username_, password))
			{
				std::cout << "auth successed\n";
				result = "auth";
				socket_.async_write_some(boost::asio::buffer(result),
					bind(&tcp_connection::handle_flag_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
			else
			{
				result = "null";
				socket_.async_write_some(boost::asio::buffer(result),
					bind(&tcp_connection::handle_auth_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}	
		}
		else
		{
			std::cout << "Error auth" << std::endl;
		}
	}
	
	void handle_register(const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (!error)
		{
			std::string result;
			std::string data(buf_.data(), bytes_transferred);
			size_t separator = data.find(':');


			username_ = data.substr(0, separator);
			std::string password = data.substr(separator + 1);

			std::cout << "User: " << username_ << std::endl;
			std::cout << "Pass: " << password << std::endl;

			if (db_server_.registerUser(username_, password))
			{
				std::cout << "User has been registered\n";
				result = "register";
				socket_.async_write_some(boost::asio::buffer(result),
					bind(&tcp_connection::handle_flag_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
			else
			{
				result = "null";
				socket_.async_write_some(boost::asio::buffer(result),
					bind(&tcp_connection::handle_register_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
		}
		else
		{
			std::cout << "Error register" << std::endl;
		}
	}

	void handle_register_read(const boost::system::error_code& error, size_t bytes_transferred)
	{
		socket_.async_read_some(boost::asio::buffer(buf_),
			bind(&tcp_connection::handle_register, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_auth_read(const boost::system::error_code& error, size_t bytes_transferred)
	{
		socket_.async_read_some(boost::asio::buffer(buf_),
			bind(&tcp_connection::handle_auth, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_create_room(const boost::system::error_code& error, size_t bytes_transferred)
	{
		std::string room_name(buf_.data(), bytes_transferred);
		db_server_.createRoom(room_name);

		db_server_.addMember(room_name, username_);

		std::cout << "Room has been added.\n";
		socket_.async_read_some(boost::asio::buffer(buf_),
			bind(&tcp_connection::handle_data, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

	}

	void handle_delete_room(const boost::system::error_code& error, size_t bytes_transferred)
	{
		std::string room_name(buf_.data(), bytes_transferred);
		db_server_.deleteRoom(room_name);

		std::cout << "Room has been deleted.\n";
		socket_.async_read_some(boost::asio::buffer(buf_),
			bind(&tcp_connection::handle_data, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_room_join(const boost::system::error_code& error, size_t bytes_transferred)
	{
		room_ = std::string(buf_.data(), bytes_transferred);

		std::vector<std::string> messages = db_server_.joinToRoom(room_);
		std::string messages_str;
		messages_str = serializerVector(messages);
		
		std::cout << "Room: " << room_ << std::endl;

		if (!messages.empty())
		{
			// Send list of messages
			socket_.async_write_some(boost::asio::buffer(messages_str),
				bind(&tcp_connection::handle_room_message_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			std::string response = "There are no messages in the room.\n";
			socket_.async_write_some(boost::asio::buffer(response),
				bind(&tcp_connection::handle_room_message_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
	}

	void handle_room_message_read(const boost::system::error_code& error, size_t bytes_transferred)
	{
		// Read user's commandi
		socket_.async_read_some(boost::asio::buffer(buf_),
			bind(&tcp_connection::handle_room_message_check, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_room_message_check(const boost::system::error_code& error, size_t bytes_transferred)
	{
		std::string message(buf_.data(), bytes_transferred);

		std::cout << "Command: " << message << std::endl;
		std::cout << "Result: " << (message == "/back") << std::endl;
		if (message == "/back")
		{	
			socket_.async_write_some(boost::asio::buffer(room_),
				bind(&tcp_connection::handle_flag_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		
		else if (message == "/add")
		{
			socket_.async_read_some(boost::asio::buffer(buf_),
				bind(&tcp_connection::handle_room_add_member, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		else if (message == "/kick")
		{
			socket_.async_read_some(boost::asio::buffer(buf_),
				bind(&tcp_connection::handle_room_kick_member, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
			
		else
		{
			db_server_.addMessage(username_, room_, message);

			socket_.async_read_some(boost::asio::buffer(buf_),
				bind(&tcp_connection::handle_room_join, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
	}

	void handle_room_kick_member(const boost::system::error_code& error, size_t bytes_transferred)
	{
		std::string user_name(buf_.data(), bytes_transferred);
		//db_server_.kickMember(room_, user_name);
		
		std::string response = user_name + " has been deleted";

		socket_.async_write_some(boost::asio::buffer(response),
			bind(&tcp_connection::handle_room_message_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_room_add_member(const boost::system::error_code& error, size_t bytes_transferred)
	{
		std::string user_name(buf_.data(), bytes_transferred);
		std::cout << "User name: " << user_name << std::endl;
		if (db_server_.addMember(room_, user_name))
		{
			std::string response = user_name + " join to the room.";
			db_server_.addMessage(username_, room_, response);
			socket_.async_read_some(boost::asio::buffer(buf_),
				bind(&tcp_connection::handle_room_join, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			socket_.async_read_some(boost::asio::buffer(buf_),
				bind(&tcp_connection::handle_room_join, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
	}

	void handle_room_add_message(const boost::system::error_code& error, size_t bytes_transferred)
	{
		std::string user_message(buf_.data(), bytes_transferred);
		db_server_.addMessage(username_, room_, user_message);

		std::string response = "message has been received";

		socket_.async_write_some(boost::asio::buffer(response),
			bind(&tcp_connection::handle_room_message_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	tcp::socket socket_;
	std::string username_;
	std::string room_;
	db_server& db_server_;
	boost::array<char, 128> buf_;
};


class tcp_server
{
public:
	tcp_server(boost::asio::io_context& io_context)
		: io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), server::s_port))
	{
		start_connect();
	}

private:

	// Listen and accept new connections
	void start_connect()
	{
		tcp_connection::pointer new_connection =
			tcp_connection::create(io_context_, db_server_);

		acceptor_.async_accept(new_connection->socket(),
			boost::bind(&tcp_server::handle_accept, this, new_connection,
				boost::asio::placeholders::error));
	}

	void handle_accept(tcp_connection::pointer new_connection,
		const boost::system::error_code& e)
	{
		if (!e)
		{
			new_connection->start();
		}
		start_connect();
	}

	boost::asio::io_context& io_context_;
	tcp::acceptor acceptor_;
	db_server db_server_;
};

int main()
{
	try
	{
		boost::asio::io_context io_context;
		tcp_server server(io_context);
		io_context.run();

	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}