#include "server.h"
#include "../service/message.h"
#include <iostream>
#include <thread>

using namespace std::placeholders;

void server::startAccept()
{
    socket_ptr newSocket(new ip::tcp::socket(_service));
    
    _acceptor.async_accept(*newSocket, [this, newSocket](boost::system::error_code ec)
    {
        if (!ec)
        {
            participant p("unknown", newSocket, _lastParticipant);
            _participants.insert(std::pair<int, participant>(_lastParticipant++, p));
            std::cout << "Connection established for client " << _lastParticipant-1 << std::endl;
            read(p);
            startAccept();
        }
        else
        {
            std::cout << "Error happened: client " << _lastParticipant << ", code " << ec << std::endl;
        }
    });
}

server::server(int port):
    _port(port),
    _endpoint(ip::tcp::v4(), _port),
    _acceptor(_service, _endpoint),
    _lastParticipant(0)
{
}

void server::start()
{
    std::thread t (&server::readCommands, this);
    startAccept();
    _service.run();
    t.join();
}

void server::readCommands()
{
    std::string command;
    std::cin >> command;
    while (true)
    {
        processCommand(command);
        std::cin >> command;
    }
}

void server::processCommand(std::string command)
{
    std::cout << "command was " << command << std::endl;
    if (command == "/kick")
    {
        std::cout << "enter user id to kick: " << std::endl;
        int id = -1;
        std::cin >> id;
        std::cout << "id entered: " << id << std::endl;
        if (_participants.count(id) > 0)
        {
            _participants.at(id).getSocket()->close();
            _participants.erase(id);
        }
        else
        {
            std::cout << "Client not found" << std::endl;
        }
    }
    else if (command == "/show")
    {
        for (int i = 0; i < _participants.size(); ++i)
        {
            std::cout << _participants.at(i).getId() << "  " << _participants.at(i).getName() << std::endl;
        }
    }
    else if (command == "/quit")
    {
        for (int i = 0; i < _participants.size(); ++i)
        {
            _participants.at(i).getSocket()->close();
        }
        exit(0);
    }
}

void server::read(participant p)
{
    std::shared_ptr<message> newMessage = std::make_shared<message>();
    std::cout << "Reading message of client " << p.getId() << std::endl;

    async_read_until(*p.getSocket(), _readbuf, '\n', [this, newMessage, p](boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (!ec)
        {
            std::cout << "Client " << p.getId() << " sent " << bytes_transferred << " bytes: ";

            std::istream is(&_readbuf);
            std::string s;
            is >> s;

            std::cout << s << std::endl;
            for (int i = 0; i < _participants.size(); ++i)
            {
                if (i != p.getId())
                {
                    write(_participants.at(i), s.c_str(), bytes_transferred);
                }
            }

            is >> s;        // clean input buffer
            read(p);
        }
        else
        {
            std::cout << "Error happened while reading: client " << p.getId() << ", code " << ec.message() << std::endl;
            p.getSocket()->close();
            _participants.erase(p.getId());
            --_lastParticipant;
        }
    });
}

void server::write(participant p, const std::string data, int len)
{
    std::string dataToSend = data + '\n';
    ++len;

    std::cout << "Sending message to client " << p.getId() << ": " << data << ", " << len << " bytes" << std::endl;
    async_write(*p.getSocket(), buffer(dataToSend, len), [this, data, p](boost::system::error_code ec, std::size_t length)
    {
        if (!ec)
        {
            std::cout << "message was sent to client " << p.getId() << ": "<< data << std::endl;
        }
        else
        {
            std::cout << "Error happened while writing: client " << p.getId() << ", code " << ec.message() << std::endl;
        }
    });
}

int main()
{
    server chatServer(12345);
    chatServer.start();
    return 0;
}
