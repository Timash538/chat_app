#pragma once

#include <server/UserRepository.h>
#include <server/Handlers.h>
#include <CommonTypes.h>
#include <server/ICommand.h>
#include <nlohmann/json.hpp>
#include <optional>


class CommandRegistry
{
public:
	CommandRegistry(
		UserRepository& userRepo,
		MessageRepository& messageRepo,
		ChatRepository& chatRepo
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

		}
	void registerCommand(const std::string& name, std::unique_ptr<ICommand>);
	nlohmann::json execute(const std::string& name, std::optional<UserID>, const nlohmann::json& args) const;
private:
	std::unordered_map<std::string, std::unique_ptr<ICommand>> m_commands;
};