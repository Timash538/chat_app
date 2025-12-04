#pragma once

#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <CommonTypes.h>

class Connection;
class CommandRegistry;

class Server
{
public:
	Server(const std::string& host, uint16_t port);
	~Server();

	bool start();
	void stop();

	// Äëÿ Connection
	void handleRequest(std::shared_ptr<Connection> conn, const nlohmann::json& request);
	void onUserDisconnected(const UserID& userId);
	void sendToUser(const UserID& userId, const nlohmann::json& msg);

private:
	void doAccept();
	
	std::string m_host;
	uint16_t m_port;
	asio::io_context m_ioContext;
	asio::ip::tcp::acceptor m_acceptor;
	std::atomic<bool> m_running{ false };
	std::thread m_ioThread;

	std::map<UserID, std::weak_ptr<Connection>> m_activeUsers;
	std::unique_ptr<CommandRegistry> m_commandRegistry;
};