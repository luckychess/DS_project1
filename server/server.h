#ifndef _SERVER_H_
#define _SERVER_H_

#include "participant.h"
#include <boost/asio.hpp>
#include <map>

using namespace boost::asio;

class server
{
public:
    server(int port);
    void start();
    void accept();
    void startAccept();

private:
    io_service                      _service;
    int                             _port;
    ip::tcp::endpoint               _endpoint;
    ip::tcp::acceptor               _acceptor;
    std::map<int, participant>      _participants;
    int                             _lastParticipant;
    boost::asio::streambuf          _readbuf;

    void readCommands();
    void processCommand(std::string command);
    void read(participant p);
    void write(participant p, const std::string data, int len);
};

#endif // _SERVER_H_
