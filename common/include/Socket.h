#pragma once

#include <string>
#include <cstdint>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

typedef int socklen_t;
#define CLOSE_SOCKET closesocket
#define GET_LAST_ERROR WSAGetLastError()
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#define CLOSE_SOCKET close
#define GET_LAST_ERROR errno
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#define MESSAGE_LENGTH 1024
#define PORT 7777

class Socket {
public:
    Socket();
    ~Socket();

    // Запрещаем копирование
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    bool connect(const std::string& host, uint16_t port);
    bool bind(const std::string& iface, uint16_t port);
    bool listen(int backlog = 5);
    Socket accept();

    int send(const void* data, size_t length);
    int recv(void* buffer, size_t length);

    void closeSocket();
    bool isValid() const { return _socket != INVALID_SOCKET; }
    int getDescriptor() const { return _socket; }

    static bool initialize();
    static void cleanup();

private:
    int _socket;
    bool initialized;

    void createSocket();
    sockaddr_in resolveAddress(const std::string& host, uint16_t port) const;
};