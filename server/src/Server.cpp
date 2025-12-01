#include "Server.h"
#include <iostream>
#include <csignal>
#include <memory>
#include <asio/write.hpp>

static std::atomic<bool> g_interrupted{ false };

void signalHandler(int) { g_interrupted = true; }

Server::Server(const std::string& host, uint16_t port)
    : _host(host), _port(port), _acceptor(_ioContext)
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
}

Server::~Server() { 
    
    stop(); 
    _threadPool.stop();
}

bool Server::start(RequestHandler handler) {
    _threadPool.start();
    if (_running.exchange(true)) return false;

    _handler = std::move(handler);

    asio::ip::tcp::resolver resolver(_ioContext);
    auto ep = *resolver.resolve(_host, std::to_string(_port)).begin();
    _acceptor.open(ep.endpoint().protocol());
    _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(ep);
    _acceptor.listen();

    std::cout << "Server started on " << _host << ":" << _port << "\n";

    doAccept();
    _ioThread = std::thread([this] { _ioContext.run(); });

    while (_running && !g_interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    stop();
    return true;
}

void Server::stop() {
    if (!_running.exchange(false)) return;
    _ioContext.stop();
    if (_ioThread.joinable()) _ioThread.join();
    std::cout << "Server stopped\n";
}

void Server::doAccept() {
    _acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
        if (!ec && _running) {
            std::cout << "Client connected\n";
            auto sharedSocket = std::make_shared<asio::ip::tcp::socket>(std::move(socket));
            doRead(sharedSocket);
        }
        if (_running) doAccept();
        });
}

void Server::doRead(std::shared_ptr<asio::ip::tcp::socket> socket) {
    auto buf = std::make_shared<std::vector<char>>(4096);
    socket->async_read_some(asio::buffer(*buf),
        [this, socket, buf](error_code ec, size_t n) {
            if (ec || n == 0 || !_running) return;

            std::string msg(buf->data(), n);
            _threadPool.push_task([this, socket, msg = std::move(msg)] {
                if (!_running) return;
                auto reply = _handler(msg) + "\n";
                asio::post(_ioContext, [socket, reply = std::move(reply)]() mutable {
                    if (socket->is_open()) {
                        auto data = std::make_shared<std::string>(std::move(reply));
                        asio::async_write(*socket, asio::buffer(*data),
                            [data](error_code, size_t) {});
                    }
                    });
            });

            doRead(socket);
        });
}