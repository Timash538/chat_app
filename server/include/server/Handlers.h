#pragma once
#include <optional>
#include <CommonTypes.h>
#include <server/ICommand.h>
#include <server/UserRepository.h>
#include <server/ChatRepository.h>
#include <server/MessageRepository.h>

class CommandRegistry;

class LoginHandler : public ICommand
{
public:
	explicit LoginHandler(UserRepository& userRepo) : m_userRepo(userRepo) {}
	nlohmann::json execute(std::optional<UserID> userID, const nlohmann::json& req) override;
private:
	UserRepository& m_userRepo;
};

class RegisterHandler : public ICommand
{
public:
	explicit RegisterHandler(UserRepository& userRepo) : m_userRepo(userRepo) {}
	nlohmann::json execute(std::optional<UserID> userID, const nlohmann::json& req);
private:
	UserRepository& m_userRepo;
};

class FetchUsersHandler : public ICommand
{
public:
	explicit FetchUsersHandler(UserRepository& userRepo) : m_userRepo(userRepo) {}
	nlohmann::json execute(std::optional<UserID> userID, const nlohmann::json& req);
private:
	UserRepository& m_userRepo;
};

class SendMessageHandler : public ICommand
{
public:
	explicit SendMessageHandler(MessageRepository& messageRepo) : m_messageRepo(messageRepo) {}
	nlohmann::json execute(std::optional<UserID> userID, const nlohmann::json& req);
private:
	MessageRepository& m_messageRepo;
};

class FetchMessageHistoryHandler : public ICommand
{
public:
	explicit FetchMessageHistoryHandler(MessageRepository& messageRepo) : m_messageRepo(messageRepo) {}
	nlohmann::json execute(std::optional<UserID> userID, const nlohmann::json& req);
private:
	MessageRepository& m_messageRepo;
};

class CreateChatHandler : public ICommand
{
public:
	explicit CreateChatHandler(ChatRepository& chatRepo) : m_chatRepo(chatRepo) {}
	nlohmann::json execute(std::optional<UserID> userID, const nlohmann::json& req);
private:
	ChatRepository& m_chatRepo;
};

class FetchChatsHandler : public ICommand
{
public:
	explicit FetchChatsHandler(ChatRepository& chatRepo) : m_chatRepo(chatRepo) {}
	nlohmann::json execute(std::optional<UserID> userID, const nlohmann::json& req);
private:
	ChatRepository& m_chatRepo;
};

class FetchChatHandler : public ICommand
{
public:
	explicit FetchChatHandler(ChatRepository& chatRepo) : m_chatRepo(chatRepo) {}
	nlohmann::json execute(std::optional<UserID> userID, const nlohmann::json& req);
private:
	ChatRepository& m_chatRepo;
};


