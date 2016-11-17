#include "server.h"
#include <iostream>

using namespace std::placeholders;

void server::startAccept()
{
    socket_ptr newSocket(new ip::tcp::socket(_service));
    participant p("unknown", newSocket, _lastParticipant);
    _participants.insert(std::pair<int, participant>(_lastParticipant++, p));
    _acceptor.async_accept(*newSocket, [this](boost::system::error_code ec)
    {
        if (!ec)
        {
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
    startAccept();
    _service.run();
}

int main()
{
    server chatServer(12345);
    chatServer.start();
    return 0;
}
