#pragma once

#include <asio.hpp>
#include <functional>
#include <atomic>
#include <memory>
#include <ThreadPool.h>

class Server {
public:
    // Обработчик получает сокет и ОДНО сообщение, но может читать дальше
    using RequestHandler = std::function<std::string(const std::string&)>;

    Server(const std::string& host, uint16_t port);
    ~Server();

    bool start(RequestHandler handler);
    void stop();

private:
    void doAccept();
    void doRead(std::shared_ptr<asio::ip::tcp::socket> socket);

    std::string _host;
    uint16_t _port;
    asio::io_context _ioContext;
    asio::ip::tcp::acceptor _acceptor;
    std::atomic<bool> _running{ false };
    std::thread _ioThread;
    ThreadPool _threadPool;
    std::function<std::string(const std::string&)> _handler;
};