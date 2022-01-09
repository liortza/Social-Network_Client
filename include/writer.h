#ifndef CLIENT_WRITER_H
#define CLIENT_WRITER_H

#include "connectionHandler.h"

class Writer {
private:
    ConnectionHandler &_handler;
    bool &_shouldTerminate;
    bool &_logout;

public:
    Writer(ConnectionHandler &handler, bool &shouldTerminate, bool &logout);
    void write();
};


#endif //CLIENT_WRITER_H
