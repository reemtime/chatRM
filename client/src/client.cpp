#include "client.hpp"

Client::Client(boost::asio::io_context& io_context,const boost::asio::ip::tcp::resolver::results_type& endpoints) : io_context_(io_context), socket_(io_context)
{
    connect(endpoints);
}

Client::~Client()
{
    disconnect();
}

void Client::close()
{
    boost::asio::post(io_context_, [this]() { socket_.close(); });
}

void Client::connect(const boost::asio::ip::tcp::resolver::results_type& endpoints)
{
    boost::asio::async_connect(socket_, endpoints,
    [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
    {
        if (!ec)
        {
            std::cout << "Connected to server" << std::endl;

            while(true)
            {
                std::string command;
                
                clear_screen();
                std::cout << "1. Register\n2. Log in \n";
                std::getline(std::cin, command);

                if(command == "1")
                {
                    registration();
                    break;
                }
                else if(command == "2")
                {
                    authentication();
                    break;
                }
                else
                    continue;
            }            
            send_message();
        }
        else
        {
            close();
        }
    });
}

void Client::user_input()
{
    std::string user_input;
    std::string command;
    std::string argument;          
    
    std::cout << "Enter message: ";
    std::getline(std::cin, user_input);
    std::istringstream iss(user_input);
    iss >> command >> argument;

    if(command == "/help")
    {
        help();
    }
    else
    {
        clear_message();
        client_msg_.status = 0;
        client_msg_.command = command;
        client_msg_.data.push_back(argument);
        send_message();
    }

}

void Client::room_message()
{
    std::string command;
    std::string argument; 
    std::string user_message;
    clear_message();

    std::cout << "Enter message: ";
    std::getline(std::cin, user_message);
    std::istringstream iss(user_message);
    iss >> command >> argument;
    if(command == "/add")
    {
        client_msg_.command = "/add";
        client_msg_.data.push_back(argument);
    }
    else if(command == "/help")
    {
        help();
    }
    else if(command =="/exit")
    {
        disconnect();
    }
    else if(command == "/kick")
    {
        client_msg_.command = "/kick";
        client_msg_.data.push_back(argument);
    }
    else if(command == "/menu")
    {
        client_msg_.command = "/menu";
        client_msg_.data.push_back(argument);
    }
    else
    {
        client_msg_.command = "/message";
        client_msg_.data.push_back(user_message);
    }                     
}

void Client::send_message()
{
    socket_.async_write_some(boost::asio::buffer(boost::serialization::serialize_message(client_msg_)),
    [this](boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (!ec)
        {   
            receive_message();
        }
        else
        {
            close();
        }
    });       
}

void Client::receive_message()
{
    socket_.async_read_some(boost::asio::buffer(buf_),
    [this](boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (!ec)
        {
            std::string message(buf_.data(), bytes_transferred);
            server_msg_ = boost::serialization::deserialize_message(message);
            if(server_msg_.status == 0)
            {
                clear_screen();
                if(server_msg_.command == "/join")
                {
                    room_ = client_msg_.data.at(0);
                    std::cout << "\tRoom <" << room_ << ">" <<std::endl;
                    for(auto message : server_msg_.data)
                    {
                        std::cout << message << std::endl;
                    }
                    room_message();
                    send_message();
                }
                else if(server_msg_.command == "auth")
                {   
                    if(server_msg_.data.at(0) == "error")                     
                    {
                        authentication();
                        send_message();                               
                    }
                    else
                    {
                        client_msg_.command = "/menu";
                        send_message();
                    }
                }
                else if(server_msg_.command == "register")
                {
                    if(server_msg_.data.at(0) == "error")
                    {
                        registration();
                        send_message(); 
                    }
                    else
                    {
                        authentication();
                        send_message();
                    }
                }
                else if(server_msg_.command == "/menu")
                {
                    std::cout << "\tRooms:\n";
                    for(auto room : server_msg_.data)
                    {
                        std::cout << room << std::endl;
                    }
                    user_input();

                }
                else if(server_msg_.command == "/create")
                {
                    
                }
                else if(server_msg_.command == "/delete")
                {
                    
                }                   
                else if(server_msg_.command == "/add")
                {
                    if(server_msg_.data.at(0) == "error")
                    {
                        std::cerr << "User has not been added";
                    }
                    else
                    {
                        client_msg_.command = "/join";
                        client_msg_.data.push_back("join");
                        send_message();
                    }
                }
                else if(server_msg_.command == "/kick")
                {
                    if(server_msg_.data.at(0) == "error")
                    {
                        std::cerr << "User has not been kicked";
                    }
                    else
                    {
                        client_msg_.command = "/join";
                        client_msg_.data.push_back("/join");
                        send_message();
                    }
                }    
            }
            else if(server_msg_.status == 1)
            {
                error_message(server_msg_.data.at(0));
            }
        }
        else
        {
            close();
        }
    });       
}

void Client::clear_screen()
{
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
}

void Client::clear_message()
{
    server_msg_.status = 0;
    server_msg_.command = "";
    server_msg_.data.clear();

    client_msg_.status = 0;
    client_msg_.command = "";
    client_msg_.data.clear();
}

void Client::authentication()

{
    clear_screen();
    clear_message();
    std::string username;
    std::string password;

    std::cout << "Log in \n";
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    std::cout << "Enter your password: ";
    std::getline(std::cin, password);

    client_msg_.command = "auth";
    client_msg_.data.push_back(username);
    client_msg_.data.push_back(password);

}

void Client::registration()
{
    clear_screen();
    clear_message();
    std::string username;
    std::string password;

    std::cout << "Register\n";
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    std::cout << "Enter your password: ";
    std::getline(std::cin, password);

    client_msg_.command = "register";
    client_msg_.data.push_back(username);
    client_msg_.data.push_back(password);

}

void Client::error_message(std::string& error)
{
    clear_screen();
    clear_message();
    std::cout << "Error: " << error << std::endl;
    system("pause");
    clear_screen();
    user_input();
}

void Client::help()
{
    std::cout<< "\tMenu commands:\n" 
    "/join <room name> - enter the room\n"
    "/create <room name> - create the room\n"
    "/delete <room name> - delete the room\n"
    "\tRoom commands:\n"
    "/menu - exit the menu\n"
    "/add <user name> - add user to the room\n"
    "/kick <user name> - remove user from the room" << std::endl;
    user_input();
}

void Client::disconnect()
{
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close(ec);

    if (!ec)
    {
        std::cout << "Disconnected from server" << std::endl;
    }
    else
    {
        std::cerr << "Error : " << ec.message() << std::endl;
    }
}

