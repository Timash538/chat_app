#include "server/Server.h"
#include "server/Connection.h"
#include "server/CommandRegistry.h"
#include <iostream>
#include <csignal>
#include <memory>

static std::atomic<bool> g_interrupted{ false };
void signalHandler(int) { g_interrupted = true; }

Server::Server(const std::string& host, uint16_t port)
    : m_host(host), m_port(port), m_acceptor(m_ioContext),
    m_commandRegistry(std::make_unique<CommandRegistry>()) {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
}

Server::~Server() {
    stop();
}

bool Server::start() {
    if (m_running.exchange(true)) return false;

    asio::ip::tcp::resolver resolver(m_ioContext);
    auto ep = *resolver.resolve(m_host, std::to_string(m_port)).begin();
    m_acceptor.open(ep.endpoint().protocol());
    m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(ep);
    m_acceptor.listen();

    std::cout << "Server started on " << m_host << ":" << m_port << "\n";
    doAccept();

    m_ioThread = std::thread([this] { m_ioContext.run(); });

    while (m_running && !g_interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    stop();
    return true;
}

void Server::stop() {
    if (!m_running.exchange(false)) return;
    m_ioContext.stop();
    if (m_ioThread.joinable()) m_ioThread.join();
    std::cout << "Server stopped\n";
}

void Server::doAccept() {
    if (!m_running) return;

    m_acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
        if (!ec && m_running) {
            auto conn = Connection::create(m_ioContext, this);
            conn->socket() = std::move(socket);
            conn->start();
        }
        if (m_running) doAccept();
        });
}

void Server::handleRequest(std::shared_ptr<Connection> conn, const nlohmann::json& req) {
    std::string cmd = req.value("cmd", "");

    if (cmd == "login") {
        auto user = req.value("user", "");
        auto pass = req.value("pass", "");
        if (user.empty() || pass.empty()) {
            conn->send({ {"error", "missing user or pass"} });
            return;
        }
        UserID userID = 0;
        // Здесь — проверка в БД или in-memory registry
        // Для примера — принимаем любого
        conn->setAuthenticated(userID);
        m_activeUsers[userID] = conn;

        // Отправляем подтверждение + список онлайн
        std::vector<UserID> online;
        for (const auto& [u, c] : m_activeUsers) {
            if (auto c_ptr = c.lock()) online.push_back(u);
        }
        conn->send({ {"status", "ok"}, {"user", user}, {"online", online} });

        // Уведомляем всех, что зашёл новый юзер
        for (auto& [u, c] : m_activeUsers) {
            if (auto c_ptr = c.lock(); c_ptr != conn) {
                c_ptr->send({ {"cmd", "user_joined"}, {"user", user} });
            }
        }
        return;
    }
    // Требуется авторизация
    if (!conn->authenticated()) {
        conn->send({ {"error", "not authenticated"} });
        return;
    }

    // Делегируем команды
    try {
        auto response = m_commandRegistry->execute(cmd, conn->userId(), req);
        conn->send(response);
    }
    catch (const std::exception& e) {
        conn->send({ {"error", std::string("command failed: ") + e.what()} });
    }
}

void Server::onUserDisconnected(const UserID& userId) {
    m_activeUsers.erase(userId);

    // Уведомляем всех об уходе
    for (auto& [u, c] : m_activeUsers) {
        if (auto c_ptr = c.lock()) {
            c_ptr->send({ {"cmd", "user_left"}, {"user", userId} });
        }
    }
}

void Server::sendToUser(const UserID& userId, const nlohmann::json& msg) {
    auto it = m_activeUsers.find(userId);
    if (it != m_activeUsers.end()) {
        if (auto conn = it->second.lock()) {
            conn->send(msg);
        }
        else {
            m_activeUsers.erase(it);
        }
    }
}