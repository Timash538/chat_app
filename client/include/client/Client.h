#pragma once

#include <Socket.h>
#include <string>

class Client
{
public:
	Client();
	~Client();

    bool connect(const std::string& host, uint16_t port);

    std::string sendRequest(const std::string& request);
    std::string sendSimpleRequest(const std::string& request);

    //bool sendOnly(const std::string& request);

    //std::string receiveResponse();

    void close();

    //bool isConnected() const { return _socket.isValid(); }

private:
    Socket _socket;
};