#include "Server.h"

void onMessage(asio::ip::tcp::socket& client, const std::string& msg) {
    if (msg == "quit\n") {
        asio::write(client, asio::buffer("Bye!\n"));
        // Не закрываем сокет здесь — сервер сам закроет при ошибке чтения
        return;
    }

    std::string response = "Echo: " + msg;
    asio::write(client, asio::buffer(response));
}

int main() {
    Server server("0.0.0.0", 7777);
    server.start(onMessage);
}