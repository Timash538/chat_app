#pragma once

#include <asio.hpp>
#include <functional>
#include <atomic>
#include <memory>

class Server {
public:
    // Обработчик получает сокет и ОДНО сообщение, но может читать дальше
    using MessageHandler = std::function<void(asio::ip::tcp::socket&, const std::string&)>;

    Server(const std::string& host, uint16_t port);
    ~Server();

    bool start(MessageHandler handler);
    void stop();

private:
    void doAccept();
    void doRead(std::shared_ptr<asio::ip::tcp::socket> socket);

    std::string _host;
    uint16_t _port;
    asio::io_context _ioContext;
    asio::ip::tcp::acceptor _acceptor;
    MessageHandler _handler;
    std::atomic<bool> _running{ false };
    std::thread _ioThread;
};