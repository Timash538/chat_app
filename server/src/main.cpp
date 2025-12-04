#include "server/Server.h"
#include <iostream>
#include <libpq-fe.h>

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    //PQsetTraceFlags(0); // отключить отладку
    auto conn = PQconnectdb("host=localhost dbname=chat_dev user=dev password=devpass");

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection failed: " << PQerrorMessage(conn);
        PQfinish(conn);
        return 1;
    }

    auto res = PQexec(conn, "SELECT id, username FROM users");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(conn);
        PQclear(res);
        PQfinish(conn);
        return 1;
    }

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(conn);
        PQclear(res);
        PQfinish(conn);
        return 1;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        std::cout << "ID: " << PQgetvalue(res, i, 0)
            << ", Username: " << PQgetvalue(res, i, 1) << "\n";
    }

    PQclear(res);
    PQfinish(conn);
    return 0;

    //Server server("0.0.0.0", 9955);
    //server.start();
}