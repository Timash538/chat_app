#pragma once

#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include <atomic>
#include <memory>
#include <vector>
#include <string>
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

	void handleRequest(std::shared_ptr<Connection> conn, const nlohmann::json& request);
	void onUserDisconnected(const uint64_t& userId);
	
private:
	void doAccept();
	bool respondOnAdminRequests(const std::shared_ptr<Connection>& weak_conn, const nlohmann::json& req); //админские запросы (пока не проверяем безопасность итд)
	bool respondOnUserRequests(const std::shared_ptr<Connection>& weak_conn, const nlohmann::json& req);
	bool newMessageNotify(const uint64_t& chat_id, const nlohmann::json& response);
	bool newChatNotify(const uint64_t& chat_id);
	void newUserNotify();
	
	std::string m_host;
	uint16_t m_port;
	asio::io_context m_ioContext;
	asio::ip::tcp::acceptor m_acceptor;
	std::atomic<bool> m_running{ false };
	std::thread m_ioThread;
	ThreadPool m_threadPool;

	//БД и все что с ними связано
	Database m_db;
	UserRepository m_userRepo;
	MessageRepository m_messageRepo;
	ChatRepository m_chatRepo;
	AdminRepository m_adminRepo;

	
	std::mutex m_activeMutex; //mutex для обращения к m_activeUsers
	std::map<uint64_t, std::weak_ptr<Connection>> m_activeUsers; //Удобно дисконнектить и оповещать юзеров онлайн
	std::unique_ptr<CommandRegistry> m_commandRegistry; //Чтобы handleRequest не был всратым
};