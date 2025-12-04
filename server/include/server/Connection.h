#pragma once

#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <atomic>
#include <deque>
#include <memory>
#include <CommonTypes.h>

class Server;

class Connection : public std::enable_shared_from_this<Connection> {
public:
    static std::shared_ptr<Connection> create(asio::io_context& io_ctx, Server* server);

    asio::ip::tcp::socket& socket() { return _socket; }
    void start();
    void send(const nlohmann::json& msg);

    // Auth
    void setAuthenticated(UserID userId);
    bool authenticated() const { return _authenticated; }
    const UserID userId() const { return _userId; }

private:
    Connection(asio::io_context& io_ctx, Server* server);
    void doRead();
    void doWrite();
    void close();

    asio::ip::tcp::socket _socket;
    Server* _server;
    asio::streambuf _readBuf;
    std::deque<std::string> _writeQueue;
    std::atomic<bool> _authenticated{ false };
    UserID _userId;
};