#ifndef CLIENT_READER_H
#define CLIENT_READER_H

#include "connectionHandler.h"

class Reader {
private:
    ConnectionHandler &_handler;
    bool &_shouldTerminate;

public:
    Reader(ConnectionHandler &handler, bool &shouldTerminate);
    void read();
};


#endif //CLIENT_READER_H
