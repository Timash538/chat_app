#include <client/Client.h>
#include <iostream>

Client::Client(){}

Client::~Client()
{
	close();
}

bool Client::connect(const std::string& host, uint16_t port)
{

    std::cout << "Connecting to " << host << ":" << port << "..." << std::endl;

    if (!_socket.connect(host, port)) {
        std::cerr << "Failed to connect to " << host << ":" << port << std::endl;
        return false;
    }

    std::cout << "Connected successfully!" << std::endl;
    return true;
}

std::string Client::sendRequest(const std::string& request)
{

    int sent = _socket.send(request.c_str(), request.size());
    if (sent <= 0) {
        std::cerr << "Failed to send request" << std::endl;
        return "";
    }

    const size_t bufferSize = 4096;
    char buffer[bufferSize];
    std::string response;

    int received = _socket.recv(buffer, bufferSize - 1);
    if (received <= 0) {
        std::cerr << "Failed to receive response" << std::endl;
        return "";
    }

    buffer[received] = '\0';
    response = buffer;

    return response;
}

void Client::close() {
   // if (isConnected()) 
    {
        _socket.closeSocket();
        std::cout << "Connection closed" << std::endl;
    }
}