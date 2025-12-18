#pragma once

#include <optional>
#include <Models.h>
#include <server/Database.h>
#include <string>
#include <optional>

//Доступ к БД для вывода чатов
class ChatRepository
{
public:
	explicit ChatRepository(Database& db) : m_db(db) {}
	ChatList getChatPreviewsForUser(const uint64_t& user_id);
	ChatFull getChatForUser(const uint64_t& user_id, const uint64_t& chat_id);
	std::vector<uint64_t> getUsersFromChat(const uint64_t& chat_id);
	uint64_t createChat(
		const std::vector<uint64_t>& user_ids,
		const std::string& type,
		const std::optional<std::string>& name);
private:
	Database& m_db;
};