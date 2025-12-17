#pragma once

#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <CommonTypes.h>
#include <optional>
#include <server/UserRepository.h>
#include <server/MessageRepository.h>
#include <server/ChatRepository.h>
#include <server/Adminrepository.h>
#include <server/ThreadPool.h>

class Connection;
class CommandRegistry;

class Server : public std::enable_shared_from_this<Server>
{
public:
	Server(const std::string& host, uint16_t port);
	~Server();

	bool start();
	void stop();

	// Äëÿ Connection
	void handleRequest(std::shared_ptr<Connection> conn, const nlohmann::json& request);
	void onUserDisconnected(const UserID& userId);
	void onNewUser();

private:
	void doAccept();
	
	std::string m_host;
	uint16_t m_port;
	asio::io_context m_ioContext;
	asio::ip::tcp::acceptor m_acceptor;
	std::atomic<bool> m_running{ false };
	std::thread m_ioThread;
	ThreadPool m_threadPool;

	Database m_db;
	UserRepository m_userRepo;
	MessageRepository m_messageRepo;
	ChatRepository m_chatRepo;
	AdminRepository m_adminRepo;

	std::mutex m_activeMutex;
	std::map<UserID, std::weak_ptr<Connection>> m_activeUsers;
	std::unique_ptr<CommandRegistry> m_commandRegistry;
};