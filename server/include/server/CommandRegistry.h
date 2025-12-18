#pragma once

#include <server/Handlers.h>
#include <server/ICommand.h>
#include <nlohmann/json.hpp>
#include <optional>

// Хранитель handler'ов для Server для масштабируемости и простого добавления функционала
class CommandRegistry
{
public:
	CommandRegistry(
		UserRepository& userRepo,
		MessageRepository& messageRepo,
		ChatRepository& chatRepo,
		AdminRepository& adminRepo
	)
	{

		m_commands["login"] = std::make_unique<LoginHandler>(userRepo);
		m_commands["register"] = std::make_unique<RegisterHandler>(userRepo);
		m_commands["send_message"] = std::make_unique<SendMessageHandler>(messageRepo);
		m_commands["fetch_history"] = std::make_unique<FetchMessageHistoryHandler>(messageRepo);
		m_commands["create_chat"] = std::make_unique<CreateChatHandler>(chatRepo);
		m_commands["fetch_chats"] = std::make_unique<FetchChatsHandler>(chatRepo);
		m_commands["fetch_users"] = std::make_unique<FetchUsersHandler>(userRepo);
		m_commands["fetch_chat"] = std::make_unique<FetchChatHandler>(chatRepo);
		m_commands["admin_fetch_all"] = std::make_unique<AdminFetchAllInfo>(adminRepo);
		m_commands["admin_fetch_chat"] = std::make_unique<AdminFetchChat>(adminRepo);
		m_commands["admin_banhammer"] = std::make_unique<AdminBanhammer>(adminRepo);

		}
	void registerCommand(const std::string& name, std::unique_ptr<ICommand>);
	nlohmann::json execute(const std::string& name, std::optional<uint64_t>, const nlohmann::json& args) const;
private:
	std::unordered_map<std::string, std::unique_ptr<ICommand>> m_commands;
};