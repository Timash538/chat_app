#pragma once
#include <libpq-fe.h>
#include <string>
#include <vector>
#include <mutex>
#include <optional>

// Обработка ошибок DB
class DatabaseError : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

// Класс для запросов к БД
class Database
{
public:
	Database(const std::string& conn_str);
	~Database();
	std::vector<std::vector<std::string>> query(
		const std::string& sql, const std::vector<std::string>& params); //Возможно лучше доставать std::vector<std::unordered_map<std::string,std::string>>

private:
	PGconn* m_conn = nullptr;
	std::mutex m_mutex; // Делаю так, в будущем нужно реализовать ConnectionPool, чтобы не было очереди к БД
};