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
            read(p.getId());
            startAccept();
        }
        else
        {
            std::cout << "A new client tried to connect but error happened: code " << ec << std::endl;
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

void server::read(int participantId)
{
    std::shared_ptr<message> newMessage = std::make_shared<message>();

    async_read_until(*_participants.at(participantId).getSocket(), _readbuf, '\n',
        [this, newMessage, participantId](boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (!ec)
        {
            std::cout << "Client " << _participants.at(participantId).getName() << " sent " << bytes_transferred << " bytes: ";

            std::istream is(&_readbuf);
            std::string s;
            is >> s;
            std::cout << s << std::endl;

            if (_participants.at(participantId).getFirstMessageFlag())
            {
                _participants.at(participantId).setName(s);
                _participants.at(participantId).cleanFirstMessageFlag();
            }

            else
            {
                for (int i = 0; i < _participants.size(); ++i)
                {
                    if (i != participantId)
                    {
                        write(i, s.c_str(), bytes_transferred);
                    }
                }
            }

            is >> s;        // clean input buffer
            read(participantId);
        }
        else
        {
            std::cout << "Error happened while reading: client " << _participants.at(participantId).getName() << ", code " << ec.message() << std::endl;
            _participants.at(participantId).getSocket()->close();
            _participants.erase(participantId);
            --_lastParticipant;
        }
    });
}

void server::write(int participantId, const std::string data, int len)
{
    std::string dataToSend = data + '\n';
    ++len;

    async_write(*_participants.at(participantId).getSocket(), buffer(dataToSend, len),
        [this, data, participantId](boost::system::error_code ec, std::size_t length)
    {
        if (!ec)
        {
            std::cout << "message was sent to client " << _participants.at(participantId).getName() << ": "<< data << std::endl;
        }
        else
        {
            std::cout << "Error happened while writing: client " << _participants.at(participantId).getName() << ", code " << ec.message() << std::endl;
        }
    });
}

int main()
{
    server chatServer(12345);
    chatServer.start();
    return 0;
}
