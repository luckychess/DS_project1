#include "client.h"
#include "../service/message.h"
#include <iostream>
#include <sstream>
#include <thread>

client::client(char *ip, char *port)
{
    _serverIP = ip;
    std::stringstream portValue;
    portValue << port;
    portValue >> _port;
    start();
}

void client::start()
{
    _endpoint = ip::tcp::endpoint(ip::address::from_string(_serverIP), _port);
    _socket = std::make_shared<ip::tcp::socket>(ip::tcp::socket(_service));
    _socket->async_connect(_endpoint, [this](boost::system::error_code ec)
    {
        if (!ec)
        {
            std::cout << "Connection successfull" << std::endl;
            std::thread readThread(&client::read, this);
            readInput();
            readThread.join();
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
    std::cout << "Waiting for messages..." << std::endl;

    async_read_until(*_socket, _readbuf, '\n', [this](boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (!ec)
        {
            std::cout << "Got message of " << bytes_transferred << "bytes: ";

            std::istream is(&_readbuf);
            std::string s;
            is >> s;
            is >> s;                   // clean input buffer

            std::cout << s << std::endl;
        }
        else
        {
            std::cout << "Error happened while reading: code " << ec.message() << std::endl;
        }
    });
}

void client::write(const std::string data, int len)
{
    std::string dataToSend = data + '\n';
    ++len;

    std::cout << "Sending message: " << data << ", " << len << " bytes" << std::endl;
    async_write(*_socket, buffer(dataToSend, len), [this](boost::system::error_code ec, std::size_t length)
    {
        if (!ec)
        {
            std::cout << "Message was sent successfully" << std::endl;
        }
        else
        {
            std::cout << "Error happened during message transfer: code " << ec.message() << std::endl;
        }
    });
}

void client::readInput()
{
    std::string command;
    std::cin >> command;
    std::cout << "Command was: " << command << std::endl;
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
        std::cout << "disconnecting..." << std::endl;
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
