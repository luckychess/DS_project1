#include "participant.h"

participant::participant(std::string name, socket_ptr socket, int id):
    _name(name),
    _socket(socket),
    _id(id)
{
}

socket_ptr participant::getSocket()
{
    return _socket;
}
