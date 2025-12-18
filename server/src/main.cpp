#include "server/Server.h"
#include <iostream>
#include <server/Database.h>
#include <server/UserRepository.h>
#include <server/PasswordHash.h>
#include <libpq-fe.h>

int main() {
    try
    {
        setlocale(LC_ALL, "");
        initCrypto();

        auto server = std::make_shared<Server>("0.0.0.0", 9955);
        server->start();
    }
    catch (std::exception& e)
    {
        std::cout << e.what();
    }
}