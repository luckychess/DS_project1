#include "message.h"
#include <string.h>

message::message()
{
    _len = 512;
    _body = new char[512] {0};
}

message::message(char *newMessage, int len)
{
    strncpy(_body, newMessage, len);
    _len = len;
}

int message::getLen() const
{
    return _len;
}

char* message::getBody() const
{
    return _body;
}

