#include "server.h"
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
            startAccept();
        }
        else
        {
            std::cout << "Error happenned: client " << _lastParticipant-1 << ", code " << ec << std::endl;
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
    while (command != "/quit")
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
}

int main()
{
    server chatServer(12345);
    chatServer.start();
    return 0;
}
