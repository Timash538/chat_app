#include <Server.h>
#include <iostream>

int main()
{ 
    Server server("*",7777);

    // Определяем обработчик запросов
    auto requestHandler = [](Socket clientSocket, const std::string& request) {
        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Hello from Simple TCP Server!\n"
            "Request size: " + std::to_string(request.size()) + " bytes\n";

        clientSocket.send(response.c_str(), response.size());
        };

    // Запускаем сервер (блокирующий вызов)
    std::cout << "Starting server..." << std::endl;
    server.start(requestHandler);

    std::cout << "Server has stopped" << std::endl;
    return 0;
}