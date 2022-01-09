#include <stdlib.h>
#include <connectionHandler.h>
#include <reader.h>
#include <writer.h>
#include <thread>
#include <fstream>

using boost::asio::ip::tcp;

/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);

    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

    bool shouldTerminate = false, logout = false;
    Reader readerTask(connectionHandler, shouldTerminate, logout);
    Writer writerTask(connectionHandler, shouldTerminate, logout);

    std::thread readerThread(&Reader::read, &readerTask);
    std::thread writerThread(&Writer::write, &writerTask);

    readerThread.join();
    writerThread.join();

    return 0;
}

