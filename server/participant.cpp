#include "participant.h"

participant::participant(std::string name, socket_ptr socket, int id):
    _name(name),
    _socket(socket),
    _id(id)
{
}

participant::participant(const participant &other)
{
    _name = other.getName();
    _socket = other.getSocket();
    _id = other.getId();
}

participant& participant::operator=(const participant &other)
{
    _name = other.getName();
    _socket = other.getSocket();
    _id = other.getId();
}

socket_ptr participant::getSocket() const
{
    return _socket;
}

std::string participant::getName() const
{
    return _name;
}

int participant::getId() const
{
    return _id;
}
