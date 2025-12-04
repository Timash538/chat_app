#include "server/Connection.h"
#include "server/Server.h"
#include <iostream>

std::shared_ptr<Connection> Connection::create(asio::io_context& io_ctx, Server* server) {
    return std::shared_ptr<Connection>(new Connection(io_ctx, server));
}

Connection::Connection(asio::io_context& io_ctx, Server* server)
    : _socket(io_ctx), _server(server) {
}

void Connection::start() {
    doRead();
}

void Connection::doRead() {
    asio::async_read_until(_socket, _readBuf, "\n",
        [self = shared_from_this()](std::error_code ec, size_t bytes_transferred) {
            if (ec) {
                self->close();
                return;
            }

            std::istream is(&self->_readBuf);
            std::string line;
            if (std::getline(is, line)) {
                try {
                    //Check
                    std::cout << line << std::endl;
                    auto json = nlohmann::json::parse(line);
                    self->_server->handleRequest(self, json);
                }
                catch (const std::exception& e) {
                    std::cerr << "Malformed JSON: " << e.what() << "\n";
                    self->close();
                }
            }
            self->doRead();
        });
}

void Connection::send(const nlohmann::json& msg) {
    asio::post(_socket.get_executor(), [self = shared_from_this(), data = msg.dump() + "\n"]() mutable {
        bool write_in_progress = !self->_writeQueue.empty();
        self->_writeQueue.push_back(std::move(data));
        if (!write_in_progress) {
            self->doWrite();
        }
    });
}

void Connection::doWrite() {
    auto msg = std::make_shared<std::string>(std::move(_writeQueue.front()));
    _writeQueue.pop_front();

    asio::async_write(_socket, asio::buffer(*msg),
        [self = shared_from_this(), msg](std::error_code ec, size_t) {
            if (ec) {
                self->close();
                return;
            }
            if (!self->_writeQueue.empty()) {
                self->doWrite();
            }
        });
}

void Connection::setAuthenticated(UserID userId) {
    _authenticated = true;
    _userId = std::move(userId);
}

void Connection::close() {
    std::error_code ec;
    _socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    _socket.close(ec);
    if (_authenticated) {
        _server->onUserDisconnected(_userId);
    }
}