#include <connectionHandler.h>

using boost::asio::ip::tcp;
//using boost::range_detail::string_;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

ConnectionHandler::ConnectionHandler(string host, short port) : host_(host), port_(port), io_service_(),
                                                                socket_(io_service_), opcode(-1) {}

ConnectionHandler::~ConnectionHandler() {
    close();
}

bool ConnectionHandler::connect() {
    std::cout << "Starting connect to "
              << host_ << ":" << port_ << std::endl;
    try {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
        boost::system::error_code error;
        socket_.connect(endpoint, error);
        if (error)
            throw boost::system::system_error(error);
    }
    catch (std::exception &e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp) {
            tmp += socket_.read_some(boost::asio::buffer(bytes + tmp, bytesToRead - tmp), error);
        }
        if (error)
            throw boost::system::system_error(error);
    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        std::cout << "getBytes" << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp) {
            tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
        if (error)
            throw boost::system::system_error(error);
    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        std::cout << "sendBytes" << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getLine(std::string &line) {
    return getFrameAscii(line, ';');
}

bool ConnectionHandler::sendLine(std::string &line) {
    return sendFrameAscii(line, ';');
}

bool ConnectionHandler::getFrameAscii(std::string &frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character. 
    // Notice that the null character is not appended to the frame string.
    try {
        do {
            getBytes(&ch, 1);
            frame.append(1, ch);
        } while (delimiter != ch);
        decode(frame);
    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        std::cout << "getFrameAscii" << std::endl;
        return false;
    }
    return true;
}

void ConnectionHandler::decode(std::string &frame) {
    std::string opcode = frame.substr(0, 2);
    if (opcode == "09") { // notification
        //boost::replace_all(frame, "\0", " ");
        if (frame.at(2) == '0') frame = "NOTIFICATION PM " + frame.substr(3, frame.length() - 3);
        else frame = "NOTIFICATION Public " + frame.substr(3, frame.length() - 3);
    } else if (opcode == "10") frame = "ACK " + frame.substr(2, frame.length() - 2); // ack
    else frame = "ERROR " + frame.substr(2, frame.length() - 2); // error
}

bool ConnectionHandler::sendFrameAscii(const std::string &frame, char delimiter) {
    int index = frame.find(' ');
    std::string type = frame.substr(0, index);
    std::string message = frame.substr(index + 1);
    std::string toSend;
    bool result;
    if (type == "REGISTER") toSend = registerToBytes(message);
    else if (type == "LOGIN") toSend = loginToBytes(message);
    else if (type == "LOGOUT") toSend = logoutToBytes(message);
    else if (type == "FOLLOW") toSend = followToBytes(message);
    else if (type == "POST") toSend = postToBytes(message);
    else if (type == "PM") toSend = pmToBytes(message);
    else if (type == "LOGSTAT") toSend = logstatToBytes(message);
    else if (type == "STAT") toSend = statToBytes(message);
    else toSend = blockToBytes(message);
    char opBytes[2];
    shortToBytes(opcode, opBytes);
//    std::vector<char> bytesToSend;
//    bytesToSend.push_back(opBytes[0]);
//    bytesToSend.push_back(opBytes[1]);
//    const char* p = &bytesToSend[0];
//    for (char c: toSend) bytesToSend.push_back(c);
//    std::cout << "opcode: " << opcode << std::endl;
//    std::cout << "after to short: " << bytesToShort(opBytes) << std::endl;
//    std::cout << "message: " << toSend << std::endl;
//    for (int i = 0; i < bytesToSend.size(); i++) {
//        std::cout << "char " << i << ": " << p[i] << std::endl;
//    }
    //result = sendBytes(p, bytesToSend.size());
    result = sendBytes(opBytes, 2);
    if (!result) return false;
    return true;
    //result = sendBytes(toSend.c_str(), toSend.length());
    //if (!result) return false;
    //return sendBytes(&delimiter, 1);
}

void ConnectionHandler::shortToBytes(short num, char* bytesArr) {
    std::cout << "here" << std::endl;
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
    std::cout << "here2" << std::endl;
}

short ConnectionHandler::bytesToShort(char* bytesArr) {
    short result = (short)((bytesArr[0] & 0xff) << 8);
    result += (short)(bytesArr[1] & 0xff);
    return result;
}

// region TO_BYTES
std::string ConnectionHandler::registerToBytes(const string &message) {
    //boost::replace_all(message, " ", "\0");
    opcode = 1;
    return message;
}

std::string ConnectionHandler::loginToBytes(const string &message) {
    //boost::replace_all(message, " ", "\0");
    opcode = 2;
    return message;
}

std::string ConnectionHandler::logoutToBytes(const string &message) {
    opcode = 3;
    return "";
}

std::string ConnectionHandler::followToBytes(const string &message) {
    //boost::replace_all(message, " ", "\0");
    opcode = 4;
    return message;
}

std::string ConnectionHandler::postToBytes(const string &message) {
    opcode = 5;
    return message + "\0";
}

std::string ConnectionHandler::pmToBytes(const string &message) {
    //boost::replace_first(message, " ", "\0");
    opcode = 6;
    char time_buf[16];
    time_t now;
    time(&now);
    strftime(time_buf, 16, "%d-%m-%Y %H:%M", gmtime(&now));
    std::string time = time_buf;
    return message + "\0" + time + "\0";
}

std::string ConnectionHandler::logstatToBytes(const string &message) {
    opcode = 7;
    return "";
}

std::string ConnectionHandler::statToBytes(const string &message) {
    //boost::replace_all(message, " ", "|");
    opcode = 8;
    return message + "\0";
}

std::string ConnectionHandler::blockToBytes(const string &message) {
    opcode = 12;
    return message + "\0";
}
// endregion

// Close down the connection properly.
void ConnectionHandler::close() {
    try {
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}
