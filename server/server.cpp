#include "server.h"
#include "../service/message.h"
#include <iostream>
#include <thread>

using namespace std::placeholders;

void server::printHelp()
{
    std::cout << "Hello to the Simple Chat server!" << std::endl;
    std::cout << "Server runs on a port " << _port << "." << std::endl;
    std::cout << "Commands list:" << std::endl;
    std::cout << "/help - prints this help." << std::endl;
    std::cout << "/show - shows all cliens connected." << std::endl;
    std::cout << "/kick - kick the user with given id (asks after)." << std::endl;
    std::cout << "/quit - shutdown the server." << std::endl;
}

void server::startAccept()
{
    socket_ptr newSocket(new ip::tcp::socket(_service));
    
    _acceptor.async_accept(*newSocket, [this, newSocket](boost::system::error_code ec)
    {
        if (!ec)
        {
            participant p("unknown", newSocket, _totalClients);
            _participants.insert(std::pair<int, participant>(_totalClients, p));
            std::cout << "Connection established for client " << _totalClients << std::endl;
            ++_totalClients;
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
    _totalClients(0)
{
}

void server::start()
{
    printHelp();
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
    if (command == "/kick")
    {
        std::cout << "enter user id to kick: " << std::endl;
        int id = -1;
        std::cin >> id;
        if (_participants.count(id) > 0)
        {
            std::cout << _participants.at(id).getName() << " kicked." << std::endl;
            boost::system::error_code ec;
            _participants.at(id).getSocket()->shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
            _participants.at(id).getSocket()->close();
        }
        else
        {
            std::cout << "Client not found" << std::endl;
        }
    }
    else if (command == "/show")
    {
        for(auto iterator = _participants.begin(); iterator != _participants.end(); iterator++)
        {
            std::cout << iterator->second.getId() << "  " << iterator->second.getName() << std::endl;
        }
    }
    else if (command == "/help")
    {
        printHelp();
    }
    else if (command == "/quit")
    {
        for (int i = 0; i < _participants.size(); ++i)
        {
            boost::system::error_code ec;
            _participants.at(i).getSocket()->shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
            _participants.at(i).getSocket()->close();
        }
        exit(0);
    }
    else
    {
        std::cout << "Unknown command. Type /help to get a full command list." << std::endl;
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
                std::cout << "Client " << participantId << " is now known as " << s << std:: endl;
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
            std::cout << "Message was sent to client " << _participants.at(participantId).getName() << ": "<< data << std::endl;
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
