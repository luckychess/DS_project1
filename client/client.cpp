#include "client.h"
#include "../service/message.h"
#include <iostream>
//#include <thread>

client::client(char *ip, char *port)
{
    _serverIP = ip;
    // std::stringstream portValue;
    // portValue << port;
    // portValue >> _port;
    _port = port;
    start();
}

void client::start()
{
    //_endpoint = ip::tcp::endpoint(ip::address::from_string(_serverIP), _port);
    //ip::tcp::resolver::iterator endpoint_iterator;

    ip::tcp::resolver resolver(_service);
    auto endpoint_iterator = resolver.resolve({ _serverIP, _port });

    socket_ptr socket(new ip::tcp::socket(_service));
    _socket = socket;

    async_connect(*_socket, endpoint_iterator, [this](boost::system::error_code ec, ip::tcp::resolver::iterator)
    {
        if (!ec)
        {
            std::cout << "Connection successfull" << std::endl;
            _t = std::thread (&client::read, this);
            _t.join();
        }
        else
        {
            std::cout << "Connection unsuccessfull" << std::endl;
        }
    });
    _service.run();
}

void client::read()
{
    message newMessage;
    async_read(*_socket, buffer(newMessage.getBody(), newMessage.getLen()), [this, newMessage](boost::system::error_code ec, std::size_t length)
    {
        if (!ec || ec == boost::asio::error::eof)
        {
            std::cout << "Got message: ";
            std::cout << newMessage.getBody() << std::endl;
        }
        else
        {
            std::cout << "Error happened while reading: code " << ec.message() << std::endl;
        }
    });
}

void client::write(const std::string data, int len)
{
    async_write(*_socket, buffer(data, len), [this, data](boost::system::error_code ec, std::size_t length)
    {
        if (!ec)
        {
        }
    });
}

void client::readInput()
{
    std::cout << "123";
    std::string command;
    std::cin >> command;
    while (true)
    {
        processInput(command);
        std::cin >> command;
    }
}

void client::processInput(std::string command)
{
    if (command == "/quit")
    {
        //_t.join();
        _socket->close();
        exit(0);
    }
    else
    {
        write(command, command.size());
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: client <ip> <port>" << std::endl;
        return 0;
    }
    client newClient(argv[1], argv[2]);

    return 0;
}