#include "client.hpp"

int main()
{
    try
    {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("192.168.0.104","44444");
        Client client(io_context, endpoints);
        io_context.run();
    }
    catch(std::exception& error)
    {
        std::cerr << error.what() << std::endl;
        std::cerr << "Connecting to server ..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}