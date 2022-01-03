#ifndef CLIENT_WRITER_H
#define CLIENT_WRITER_H

#include "connectionHandler.h"

class Writer {
private:
    ConnectionHandler &_handler;
    bool &_shouldTerminate;

public:
    Writer(ConnectionHandler &handler, bool &shouldTerminate);
    void write();
};


#endif //CLIENT_WRITER_H
