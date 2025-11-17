#include <Socket.h>
#include <cstring>
#include <iostream>

Socket::Socket() : _socket(INVALID_SOCKET), initialized(false)
{
	if (!initialized)
	{
		if (!initialize())
		{
			std::cerr << "Failed to initialize socket" << std::endl;
			return;
		}
		initialized = true;
	}
	createSocket();
}

Socket::~Socket()
{
	close();
	if (initialized)
	{
		cleanup();
		initialized = false;
	}
}

Socket::Socket(Socket&& other) noexcept : _socket(other._socket), initialized(other.initialized)
{
	other._socket = INVALID_SOCKET;
	other.initialized = false;
}

Socket& Socket::operator=(Socket&& other) noexcept
{
	if (this != &other)
	{
		close();
		_socket = other._socket;
		initialized = other.initialized;
		other._socket = INVALID_SOCKET;
		other.initialized = false;
	}
	return *this;
}

bool Socket::initialize()
{
#ifdef _WIN32
	WSADATA wsaData;
	return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
	return true;
#endif
}

void Socket::cleanup()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

void Socket::createSocket()
{
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET)
	{
		std::cerr << "Failed to create socket. Error: " << GET_LAST_ERROR << std::endl;
	}
}

sockaddr_in Socket::resolveAddress(const std::string& host, uint16_t port) const
{
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0)
	{
		struct hostent* he = gethostbyname(host.c_str());
		if (!he || !he->h_addr_list[0])
		{
			std::cerr << "Failed to resolve hostname: " << host
				<< ". Error: " << GET_LAST_ERROR << std::endl;
		}
		else
		{
			addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr_list[0]);
		}
	}
	return addr;
}

bool Socket::connect(const std::string& host, uint16_t port)
{
	if (_socket == INVALID_SOCKET) {
		std::cerr << "Invalid socket" << std::endl;
		return false;
	}

	sockaddr_in addr = resolveAddress(host, port);
	if (addr.sin_addr.s_addr == 0) {
		return false;
	}

	if (::connect(_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
		std::cerr << "Connection failed to " << host << ":" << port
			<< ". Error: " << GET_LAST_ERROR << std::endl;
		return false;
	}
	return true;
}

bool Socket::bind(const std::string& iface, uint16_t port)
{
	if (_socket == INVALID_SOCKET) {
		std::cerr << "Invalid socket" << std::endl;
		return false;
	}

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (iface.empty() || iface == "0.0.0.0" || iface == "*") {
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else {
		if (inet_pton(AF_INET, iface.c_str(), &addr.sin_addr) <= 0) {
			std::cerr << "Invalid interface address: " << iface
				<< ". Error: " << GET_LAST_ERROR << std::endl;
			return false;
		}
	}

	if (::bind(_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
		std::cerr << "Failed to bind to " << iface << ":" << port
			<< ". Error: " << GET_LAST_ERROR << std::endl;
		return false;
	}
	return true;
}

bool Socket::listen(int backlog) {
	if (_socket == INVALID_SOCKET) {
		std::cerr << "Invalid socket" << std::endl;
		return false;
	}

	if (::listen(_socket, backlog) == SOCKET_ERROR) {
		std::cerr << "Failed to listen. Error: " << GET_LAST_ERROR << std::endl;
		return false;
	}
	return true;
}

Socket Socket::accept() {
	sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	int clientSocket = ::accept(_socket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);

	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Accept failed. Error: " << GET_LAST_ERROR << std::endl;
		return Socket(); // Возвращаем невалидный сокет
	}

	Socket clientSocketObj;
	clientSocketObj._socket = clientSocket;
	clientSocketObj.initialized = false; // Не инициализируем Winsock второй раз
	return clientSocketObj;
}

int Socket::send(const void* data, size_t length) {
	if (_socket == INVALID_SOCKET) {
		std::cerr << "Invalid socket" << std::endl;
		return -1;
	}

	int result = ::send(_socket, static_cast<const char*>(data), static_cast<int>(length), 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "Send failed. Error: " << GET_LAST_ERROR << std::endl;
	}
	return result;
}

int Socket::recv(void* buffer, size_t length) {
	if (_socket == INVALID_SOCKET) {
		std::cerr << "Invalid socket" << std::endl;
		return -1;
	}

	int result = ::recv(_socket, static_cast<char*>(buffer), static_cast<int>(length), 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "Receive failed. Error: " << GET_LAST_ERROR << std::endl;
	}
	else if (result == 0) {
		std::cout << "Connection closed by peer" << std::endl;
	}
	return result;
}

void Socket::close() {
	if (_socket != INVALID_SOCKET) {
		CLOSE_SOCKET(_socket);
		_socket = INVALID_SOCKET;
	}
}