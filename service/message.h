#ifndef _MESSAGE_H_
#define _MESSAGE_H_

class message
{
public:
    message();
    message(char *newMessage, int len);
    int getLen() const;
    char *getBody() const;
    
private:
    char *_body;
    int   _len;
};

#endif // _MESSAGE_H_a
