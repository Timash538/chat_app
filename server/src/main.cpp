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

        //    Database db("host=localhost dbname=chat_dev user=dev password=devpass");
        //    UserRepository uRepo(db);
        //    uRepo.registerUser("Timash12", "Timur12", hashPassword("306931"));
        //    UserFull authenticated = uRepo.authenticate("Timash", "306931").value_or<UserFull>({});

        //    std::cout << authenticated.id << " : " << authenticated.username << " : " << authenticated.login << std::endl;

        //    std::vector<UserPreview> users = uRepo.getAllUsers();
        //    for (auto i : users)
        //    {
        //        std::cout << i.id << " : " << i.username << std::endl;
        //    }
        //}
        auto server = std::make_shared<Server>("0.0.0.0", 9955);
        //Server server("0.0.0.0", 9955);
        server->start();
    }
    catch (std::exception& e)
    {
        std::cout << e.what();
    }
}