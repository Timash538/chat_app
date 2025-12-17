#pragma once

#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <atomic>
#include <deque>
#include <memory>
#include <optional>
#include <CommonTypes.h>

class Server;

class Connection : public std::enable_shared_from_this<Connection> {
public:
    static std::shared_ptr<Connection> create(asio::io_context& io_ctx, std::shared_ptr<Server> server);

    asio::ip::tcp::socket& socket() { return _socket; }
    void start();
    void send(const nlohmann::json& msg);

    // Auth
    void setAuthenticated(UserID userId);
    bool authenticated() const { return _userId.has_value(); }
    std::optional<UserID> userId() const { return _userId; }
    void close();

private:

    Connection(asio::io_context& io_ctx, std::weak_ptr<Server> server);
    void doRead();
    void doWrite();

    asio::ip::tcp::socket _socket;
    std::weak_ptr<Server> _server;
    asio::streambuf _readBuf;
    std::deque<std::string> _writeQueue;
    std::optional<UserID> _userId;
};