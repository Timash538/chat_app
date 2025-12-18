#pragma once

#include <optional>
#include <Models.h>
#include <server/Database.h>
#include <string>

//Класс для доступа к сообщениям в БД
class MessageRepository
{
public:
	explicit MessageRepository(Database& db) : m_db(db) {}
	MessageHistory fetchHistory(const uint64_t& chat_id);
	Message saveMessage(const uint64_t& chat_id, const uint64_t& sender_id, const std::string& content); // Тут же достаем готовое сообщение из БД после отправки
private:
	Database& m_db;
};