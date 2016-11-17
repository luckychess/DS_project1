#ifndef _PARTICIPANT_H_
#define _PARTICIPANT_H_

#include <boost/asio.hpp>
#include <string>

using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

class participant
{
public:
    participant(std::string name, socket_ptr socket, int id);
    participant(const participant &other);
    participant& operator=(const participant &other);

    socket_ptr getSocket() const;
    std::string getName() const;
    int getId() const;
private:
    socket_ptr        _socket;
    std::string       _name;
    int               _id;
};

#endif // _PARTICIPANT_H_