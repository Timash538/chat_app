#pragma once
#include <Socket.h>
#include <functional>

class Server
{
public:
	using RequestHandler = std::function<void(Socket clientSocket, const std::string& request)>;

	Server(const std::string& host, uint16_t port);
	~Server();

	bool start(RequestHandler handler);

	bool isRunning() const { return _running; }
		
private:
	uint16_t _port;
	std::string _host;
	Socket _serverSocket;
	bool _running;
	RequestHandler _handler;

	void handleClient(Socket clientSocket);
	static void signalHandler(int signal);
};