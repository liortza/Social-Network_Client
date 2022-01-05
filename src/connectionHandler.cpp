#include <connectionHandler.h>

using boost::asio::ip::tcp;

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
        //std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        //std::cout << "getBytes" << std::endl;
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
    std::cout << "decode" << std::endl;
    opcode = stringToOpcode(frame);
    std::cout << "frame after opcode: " << frame << std::endl;
    if (opcode == 9) { // notification
        replaceAll(frame, '\0', ' ');
        if (frame.at(2) == '0') frame = "NOTIFICATION PM " + frame.substr(1, frame.length() - 1);
        else frame = "NOTIFICATION Public " + frame.substr(1, frame.length() - 1);
    } else if (opcode == 10) { // ack
        short msgOpcode = stringToOpcode(frame);
        frame = "ACK " + msgOpcode;
    } else {
        short msgOpcode = stringToOpcode(frame);
        frame = "ERROR " + msgOpcode + frame; // error
    }
}

short ConnectionHandler::stringToOpcode(std::string &frame) {
    char opBytes[2];
    opBytes[0] = frame[0];
    opBytes[1] = frame[1];
    frame = frame.substr(2);
    return bytesToShort(opBytes);
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
    result = sendBytes(opBytes, 2);
    if (!result) return false;
    result = sendBytes(toSend.c_str(), toSend.length());
    if (!result) return false;
    return sendBytes(&delimiter, 1);
}

void ConnectionHandler::shortToBytes(short num, char* bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

short ConnectionHandler::bytesToShort(char* bytesArr) {
    std::cout << "here" << std::endl;
    short result = (short)((bytesArr[0] & 0xff) << 8);
    result += (short)(bytesArr[1] & 0xff);
    std::cout << "here 2" << std::endl;
    return result;
}

void ConnectionHandler::replaceAll(std::string &string, char toReplace, char toPut) {
    for (int i = 0; i < (int)string.length(); i++) {
        if (string[i] == toReplace) string[i] = toPut;
    }
}

// region TO_BYTES
std::string ConnectionHandler::registerToBytes(string &message) {
    replaceAll(message, ' ', '\0');
    opcode = 1;
    return message;
}

std::string ConnectionHandler::loginToBytes(string &message) {
    replaceAll(message, ' ', '\0');
    opcode = 2;
    return message;
}

std::string ConnectionHandler::logoutToBytes(string &message) {
    opcode = 3;
    return "";
}

std::string ConnectionHandler::followToBytes(string &message) {
    replaceAll(message, ' ', '\0');
    opcode = 4;
    return message;
}

std::string ConnectionHandler::postToBytes(string &message) {
    opcode = 5;
    return message;
}

std::string ConnectionHandler::pmToBytes(string &message) {
    int index = message.find(' ');
    message[index] = '\0';
    opcode = 6;
    char time_buf[16];
    time_t now;
    time(&now);
    strftime(time_buf, 16, "%d-%m-%Y %H:%M", gmtime(&now));
    std::string time = time_buf;
    return message + "\0" + time;
}

std::string ConnectionHandler::logstatToBytes(string &message) {
    opcode = 7;
    return "";
}

std::string ConnectionHandler::statToBytes(string &message) {
    replaceAll(message, ' ', '|');
    opcode = 8;
    return message;
}

std::string ConnectionHandler::blockToBytes(string &message) {
    opcode = 12;
    return message;
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
