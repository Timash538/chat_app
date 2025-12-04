#pragma once
#include <libpq-fe.h>
#include <string>
#include <vector>

class Database
{
public:
	Database(const std::string& conn_str);
	~Database();
	PGresult* exec(const std::string& query);
	std::vector<std::vector<std::string>> query(
		const std::string& sql, const std::vector<std::string>& params);

private:
	PGconn* m_conn = nullptr;
};