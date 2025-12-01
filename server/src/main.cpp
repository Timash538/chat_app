#include "Server.h"
#include "iostream"

std::string onMessage(const std::string& msg) {
    if (msg == "quit") {
        // Не закрываем сокет здесь — сервер сам закроет при ошибке чтения
        return "Bye";
    }
    std::cout << msg;
    return "Echo: " + msg;
}

int main() {
    Server server("0.0.0.0", 9955);
    server.start(onMessage);
}