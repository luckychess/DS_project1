#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <boost/asio.hpp>
#include <string>

using namespace boost::asio;

typedef std::shared_ptr<ip::tcp::socket> socket_ptr;

class client
{
public:
    client(char *ip, char *port);
private:
    char                           *_serverIP;
    int                             _port;
    io_service                      _service;
    ip::tcp::endpoint               _endpoint;
    socket_ptr                      _socket;
    boost::asio::streambuf          _readbuf;

    void start();
    void read();
    void write(const std::string data, int len);
    void readInput();
    void processInput(std::string command);
};

#endif // _CLIENT_H_
