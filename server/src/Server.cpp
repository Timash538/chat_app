#include <Server.h>

#include <iostream>
#include <signal.h>
#include <cstring>

static volatile sig_atomic_t g_interrupted = 0;

void Server::signalHandler(int signal)
{
	g_interrupted = 1;
}

Server::Server(const std::string& host, uint16_t port) : _host(host), _port(port), _running(false)
{
	signal(SIGINT, Server::signalHandler);
	signal(SIGTERM, Server::signalHandler);
}

Server::~Server()
{ }

bool Server::start(RequestHandler handler)
{
	if (isRunning())
	{
		std::cerr << "Server is already running!" << std::endl;
		return false;
	}

	_handler = handler;

	if (!_serverSocket.bind(_host, _port))
	{
		std::cerr << "Failed to bind server socket to " << _host << ":" << _port << std::endl;
		return false;
	}

	if (!_serverSocket.listen(5)) {
		std::cerr << "Failed to listen on port " << _port << std::endl;
		return false;
	}

	std::cout << "Server started on " << _host << ":" << _port << std::endl;
	std::cout << "Press Ctrl+C to stop the server" << std::endl;

	_running = true;
	g_interrupted = 0;

	while (_running && !g_interrupted) {
		Socket clientSocket = _serverSocket.accept();

		if (!clientSocket.isValid()) {
			if (g_interrupted) {
				break;
			}
			continue;
		}

		std::cout << "Client connected" << std::endl;
		handleClient(std::move(clientSocket));
		std::cout << "Client disconnected" << std::endl;
	}

	_running = false;
	_serverSocket.closeSocket();
	std::cout << "Server stopped" << std::endl;
	return true;
}

void Server::handleClient(Socket clientSocket)
{
	const size_t bufferSize = 4096;
	char buffer[bufferSize];

	int bytesRecieved = clientSocket.recv(buffer, bufferSize - 1);

	if (bytesRecieved <= 0)
	{
		return;
	}

	buffer[bytesRecieved] = '\0';
	std::string request = buffer;

	std::cout << "Recieved request (" << bytesRecieved << "bytes) << std::endl";

	_handler(std::move(clientSocket), request);
}

