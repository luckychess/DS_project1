#include "participant.h"

participant::participant(std::string name, socket_ptr socket, int id):
    _name(name),
    _socket(socket),
    _id(id),
    _firstMessageFlag(true)
{
}

participant::participant(const participant &other)
{
    _name = other.getName();
    _socket = other.getSocket();
    _id = other.getId();
    _firstMessageFlag = other.getFirstMessageFlag();
}

participant& participant::operator=(const participant &other)
{
    _name = other.getName();
    _socket = other.getSocket();
    _id = other.getId();
    _firstMessageFlag = other.getFirstMessageFlag();
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

bool participant::getFirstMessageFlag() const
{
    return _firstMessageFlag;
}

void participant::cleanFirstMessageFlag()
{
    _firstMessageFlag = false;
}

void participant::setName(std::string name)
{
    _name = name;
}
