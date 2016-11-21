#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <boost/asio.hpp>
#include <string>

using namespace boost::asio;

typedef std::shared_ptr<ip::tcp::socket> socket_ptr;

class client
{
public:
    client(char *ip, char *port, char *name);
private:
    char                           *_serverIP;
    int                             _port;
    io_service                      _service;
    ip::tcp::endpoint               _endpoint;
    socket_ptr                      _socket;
    boost::asio::streambuf          _readbuf;
    std::string                     _name;

    void startConnect();
    void read();
    void write(const std::string data, int len);
    void readCommands();
    void processInput(std::string command);
};

#endif // _CLIENT_H_
