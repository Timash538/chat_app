#include "server/Server.h"
#include "server/Connection.h"
#include "server/CommandRegistry.h"
#include <iostream>
#include <csignal>
#include <memory>
#include <string>

static std::atomic<bool> g_interrupted{ false };
void signalHandler(int) { g_interrupted = true; }

Server::Server(const std::string& host, uint16_t port)
    : m_host(host), m_port(port), m_acceptor(m_ioContext),
    m_db("host=localhost dbname=chat_dev user=dev password=devpass"),
    m_userRepo(m_db), m_messageRepo(m_db), m_chatRepo(m_db), m_adminRepo(m_db),
    m_commandRegistry(std::make_unique<CommandRegistry>(m_userRepo, m_messageRepo, m_chatRepo, m_adminRepo))
{
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
    m_threadPool.start();

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
    m_threadPool.stop();
}

void Server::doAccept() {
    if (!m_running) return;

    m_acceptor.async_accept([this, self = shared_from_this()](std::error_code ec, asio::ip::tcp::socket socket) {
        if (!ec && self->m_running) {
            auto conn = Connection::create(m_ioContext, self);
            conn->socket() = std::move(socket);
            conn->start();
        }
        asio::post(m_ioContext, [this, self] {
            if (self->m_running) doAccept();
            });
        
        });
}

void Server::handleRequest(std::shared_ptr<Connection> conn, const nlohmann::json& req) {

    std::string cmd = req.value("cmd", "");
    auto self = shared_from_this();

    m_threadPool.push_task([self,cmd, weak_conn = conn->weak_from_this(), req]
    {
        auto conn = weak_conn.lock();
        if (!conn) return;
        try {
            if (cmd == "admin_disconnect_user")// Refactor Перекинуть ссылку Server в CommandHandler и оттуда дисконнектить
            {
                if (!self->m_activeUsers.count(req["user_id"].get<uint64_t>())) return;
                auto& connOfUser = self->m_activeUsers[req["user_id"].get<uint64_t>()].lock();
                connOfUser->close();
                return;
            }
            if (cmd == "admin_fetch_all" || cmd == "admin_fetch_chat" || cmd == "admin_banhammer")
            {
                auto response = self->m_commandRegistry->execute(cmd, conn->userId(), req);
                conn->send(response);
                return;
            }
            else 
            if (cmd == "login" || cmd == "register")
            {
                auto response = self->m_commandRegistry->execute(cmd, conn->userId(), req);
                if (cmd == "register")
                {
                    conn->send(response);
                    self->onNewUser();
                    return;
                }

                UserID userID = std::stoull(response.value("user_id", ""));
                conn->setAuthenticated(userID);

                {
                    std::lock_guard<std::mutex> l(self->m_activeMutex);
                    self->m_activeUsers[userID] = conn;
                }

                conn->send(response);
                return;
            }
            else
            {
                if (!conn->authenticated()) {
                    conn->send({ {"error", "not authenticated"} });
                    return;
                }
                auto response = self->m_commandRegistry->execute(cmd, conn->userId(), req); 
                if (response["cmd"] == "new_chat") // Refactor Перекинуть ссылку Server в CommandHandler и оттуда делать оповещения
                {
                    std::vector<uint64_t> users_id = response["user_ids"];
                    for (uint64_t target_user_id : users_id) {
                        if (auto it = self->m_activeUsers.find(target_user_id);
                            it != self->m_activeUsers.end()) {
                            if (auto c_ptr = it->second.lock()) {
                                c_ptr->send(response);
                            }
                        }
                    }
                    return;
                }
                if (response["cmd"] == "new_message") // Refactor Перекинуть ссылку Server в CommandHandler и оттуда делать оповещения
                {
                    uint64_t user_id = response["user_id"];
                    std::vector<UserPreview> users = self->m_chatRepo.getChatForUser(user_id, response["message"]["chat_id"]).users;
                    for (UserPreview user : users) {
                        auto& target_user_id = user.id;
                        if (auto it = self->m_activeUsers.find(target_user_id);
                            it != self->m_activeUsers.end()) {
                            if (auto c_ptr = it->second.lock()) {
                                c_ptr->send({ {"cmd", "new_message"},{"message",response["message"]}, {"chat_id",response["message"]["chat_id"]}});
                            }
                        }
                    }
                    return;
                }
                conn->send(response);
                return;
            }
        }
        catch (const std::exception& e)
        {
            conn->send({{ "error",e.what() }});
        }
    });
}

void Server::onUserDisconnected(const UserID& userId) {

    std::lock_guard<std::mutex> l(m_activeMutex);
    m_activeUsers.erase(userId);
}

void Server::onNewUser() {
    for (auto& [u, c] : m_activeUsers) {
        if (auto c_ptr = c.lock()) {
            c_ptr->send({ {"cmd", "new_user"} });
        }
    }
}
