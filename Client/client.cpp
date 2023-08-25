#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

void join_room(boost::asio::ip::tcp::socket& socket, boost::system::error_code& error, std::string& room_name)
{
	boost::array<char, 256> buf;
	std::string message;

	std::cout << room_name << std::endl;

	boost::asio::write(socket, boost::asio::buffer(room_name), error);
	size_t r_bytes = socket.read_some(boost::asio::buffer(message), error);
	std::cout.write(buf.data(), r_bytes);
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3)
		{
			std::cout << "Usage: client <host> <port>" << std::endl;
			return 1;
		}

		boost::asio::io_context io_context;

		boost::asio::ip::tcp::resolver resolver(io_context);
		boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(argv[1], argv[2]);

		boost::asio::ip::tcp::socket socket(io_context);
		boost::asio::connect(socket, endpoints);

		boost::array<char, 128> buf;
		
		std::string flag = "auth";
		boost::system::error_code error;

		boost::asio::write(socket, boost::asio::buffer(flag), error);

		// Enter username and password

		std::string username;
		std::string password;
		int operation;

		std::cout << "1. Register\n2. Log in\n";
		std::cin >> operation;
		std::cin.clear();


		switch (operation)
		{
			case 1:
			{
				while (true)
				{
					std::system("cls");
					std::cout << "Register\n";
					std::cout << "Enter your username: ";
					std::getline(std::cin, username);
					std::cout << "Enter your password: ";
					std::getline(std::cin, password);

					boost::asio::write(socket, boost::asio::buffer(username + ':' + password), error);

					size_t r_bytes = socket.read_some(boost::asio::buffer(buf), error);
					if (std::string(buf.data(), r_bytes) == "register")
						break;
				}
			}
			case 2:
			{
				while (true)
				{
					std::system("cls");
					std::cout << "Log in \n";
					std::cout << "Enter your username: ";
					std::getline(std::cin, username);
					std::cout << "Enter your password: ";
					std::getline(std::cin, password);

					boost::asio::write(socket, boost::asio::buffer(username + ':' + password), error);

					size_t r_bytes = socket.read_some(boost::asio::buffer(buf), error);
					if (std::string(buf.data(), r_bytes) == "auth")
						break;
				}
			}
		}
		
		// Main menu
		for (;;)
		{
			std::system("cls");
			flag = "show";
			std::string rooms;

			boost::asio::write(socket, boost::asio::buffer(flag), error);
			
			// Read user's rooms
			size_t r_bytes = socket.read_some(boost::asio::buffer(buf), error);

			std::cout << "\tRooms:\n";
			int i = 0;
			std::cout.write(buf.data(), r_bytes);

			std::cout << "\nJoin a room: \"join <room_name>\"\n";
			std::cout << "Create a room: \"create <room_name>\"\n";
			std::cout << "Delete a room: \"delete <room_name>\"\n";

			// Main menu command
			std::string user_input;
			std::string room{"null"};
			std::string m_command{"null"};
			std::getline(std::cin, user_input);

			std::istringstream iss(user_input);
			iss >> m_command >> room;

			if (m_command == "join")
			{
				boost::asio::write(socket, boost::asio::buffer(m_command), error);

				// Inside the room
				for (;;)
				{
					std::system("cls");
					boost::array<char, 1024> buf_msg;
					std::string r_command;
					std::string user_name;

					std::cout << room << " BACK TO THE MENU \"/back\"\n" << std::endl;

					// Send name of room to join
					boost::asio::write(socket, boost::asio::buffer(room), error);
					
					// Show room's messages
					size_t r_bytes = socket.read_some(boost::asio::buffer(buf_msg), error);
					std::cout.write(buf_msg.data(), r_bytes);
					
					// Split the message
					std::getline(std::cin, user_input);
					iss = std::istringstream(user_input);
					iss >> r_command >> user_name;

					if (r_command == "/back")
					{
						// Send command "/back"
						boost::asio::write(socket, boost::asio::buffer(r_command), error);
						
						// Receive a room name
						size_t r_bytes = socket.read_some(boost::asio::buffer(buf), error);
						break;
					}

					else if (r_command == "/add")
					{
						// Send command "/add"
						boost::asio::write(socket, boost::asio::buffer(r_command), error);
						
						// Send user name to add to the room
						std::cout << "User name: " << user_name << std::endl;
						boost::asio::write(socket, boost::asio::buffer(user_name), error);
						
						continue;
					}

					else if (r_command == "/kick")
					{
						// Send command "/kick"
						boost::asio::write(socket, boost::asio::buffer(r_command), error);
						
						// Send user name to kick it from the room
						boost::asio::write(socket, boost::asio::buffer(user_name), error);
						size_t r_bytes = socket.read_some(boost::asio::buffer(buf), error);
						std::cout.write(buf.data(), r_bytes);
					}

					else
					{
						// Send a message
						boost::asio::write(socket, boost::asio::buffer(user_input), error);
					}

				}
				continue;
			}
			else if (m_command == "create")
			{
				boost::asio::write(socket, boost::asio::buffer(m_command), error);

				boost::asio::write(socket, boost::asio::buffer(room), error);
			}
			else if (m_command == "delete")
			{
				boost::asio::write(socket, boost::asio::buffer(m_command), error);

				boost::asio::write(socket, boost::asio::buffer(room), error);
			}
			else
			{
				continue;
			}
		}

		for (;;)
		{

			size_t r_byts = socket.read_some(boost::asio::buffer(buf), error);
			std::cout.write(buf.data(), r_byts);

			if (error == boost::asio::error::eof)
				break;
			else if (error)
				throw boost::system::system_error(error);
		}
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}