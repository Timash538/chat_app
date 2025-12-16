#pragma once
#include <libpq-fe.h>
#include <string>
#include <vector>
#include <mutex>
#include <optional>

class DatabaseError : public std::runtime_error {
public:
	using std::runtime_error::runtime_error; // наследуем конструкторы
};

class Database
{
public:
	Database(const std::string& conn_str);
	~Database();
	std::vector<std::vector<std::string>> query(
		const std::string& sql, const std::vector<std::string>& params);

private:
	PGconn* m_conn = nullptr;
	std::mutex m_mutex;
};