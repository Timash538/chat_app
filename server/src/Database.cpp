#include "server/Database.h"
#include <iostream>

Database::Database(const std::string& conn_str) {
    m_conn = PQconnectdb(conn_str.c_str());

    if (!m_conn) {
        throw DatabaseError("Failed to allocate connection");
    }

    if (PQstatus(m_conn) != CONNECTION_OK) {
        std::string err = PQerrorMessage(m_conn);
        PQfinish(m_conn);
        throw DatabaseError(err);
    }
}

Database::~Database() {
    if (m_conn) {
        PQfinish(m_conn);
    }
}

std::vector<std::vector<std::string>> Database::query(
    const std::string& sql,
    const std::vector<std::string>& params
) {
    if (!m_conn) {
        throw DatabaseError("Database connection is closed");
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    const size_t param_count = params.size();
    std::vector<const char*> param_values(param_count);
    std::vector<int> param_lengths(param_count);
    std::vector<int> param_formats(param_count, 0);

    for (size_t i = 0; i < param_count; ++i) {
        param_values[i] = params[i].c_str();
        param_lengths[i] = static_cast<int>(params[i].size());
    }

    PGresult* res = PQexecParams(
        m_conn,
        sql.c_str(),
        static_cast<int>(param_count),
        nullptr,             
        param_values.data(),
        param_lengths.data(),
        param_formats.data(),
        0 
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(m_conn);
        PQclear(res);
        throw DatabaseError("Query failed: " + error + "\nQuery: " + sql);
    }

    const int rows = PQntuples(res);
    const int cols = PQnfields(res);
    std::vector<std::vector<std::string>> result;
    result.reserve(rows);

    for (int i = 0; i < rows; ++i) {
        std::vector<std::string> row;
        row.reserve(cols);
        for (int j = 0; j < cols; ++j) {
            if (PQgetisnull(res, i, j)) {
                row.push_back("");
            }
            else {
                row.emplace_back(PQgetvalue(res, i, j));
            }
        }
        result.push_back(std::move(row));
    }
    PQclear(res);
    return result;
}