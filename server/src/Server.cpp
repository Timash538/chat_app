#include "Server.h"
#include <iostream>
#include <csignal>
#include <memory>

static std::atomic<bool> g_interrupted{ false };

void signalHandler(int) { g_interrupted = true; }

Server::Server(const std::string& host, uint16_t port)
    : _host(host), _port(port), _acceptor(_ioContext)
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
}

Server::~Server() { stop(); }

bool Server::start(MessageHandler handler) {
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
    const size_t maxMessageSize = 4096;
    auto buffer = std::make_shared<std::vector<char>>(maxMessageSize);

    socket->async_read_some(asio::buffer(*buffer), [this, socket, buffer](
        std::error_code ec, size_t bytesTransferred) {
            if (!ec && bytesTransferred > 0 && _running) {
                (*buffer)[bytesTransferred] = '\0';
                std::string message(buffer->data(), bytesTransferred);

                std::cout << "Received (" << bytesTransferred << "): " << message << "\n";

                // Передаём сообщение обработчику
                if (_handler) {
                    _handler(*socket, message);
                }

                // Читаем следующее сообщение
                doRead(socket);
            }
            else {
                std::cout << "Client disconnected\n";
                socket->close();
            }
        });
}