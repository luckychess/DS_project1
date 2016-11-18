#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <boost/asio.hpp>
#include <string>
#include <thread>

using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

class client
{
public:
    client(char *ip, char *port);
private:
    char                           *_serverIP;
    io_service                      _service;
    char*                             _port;
    ip::tcp::endpoint               _endpoint;
    socket_ptr                      _socket;
    std::thread                     _t;

    void start();
    void read();
    void write(const std::string data, int len);
    void readInput();
    void processInput(std::string command);
};

#endif // _CLIENT_H_