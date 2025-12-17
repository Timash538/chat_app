#pragma once

#include <optional>
#include <Models.h>
#include <server/Database.h>
#include <string>
#include <optional>

class AdminRepository
{
public:
	explicit AdminRepository(Database& db) : m_db(db) {}
	std::vector<UserFull> fetchAllUsers();
	std::vector<ChatFull> fetchAllChats();
	ChatFull fetchChat(const uint64_t& chat_id);
	MessageHistory fetchHistory(const uint64_t& chat_id);
	bool BanHammer(uint64_t user_id);
private:
	Database& m_db;
};