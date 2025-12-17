#include <server/Handlers.h>
#include <server/PasswordHash.h>
#include <exception>
#include <iostream>

nlohmann::json LoginHandler::execute(std::optional<UserID> userID, const nlohmann::json& req)
{
    auto login = req.value("login", "");
    auto pass = req.value("pass", "");

    if (login.empty() || pass.empty()) {
        throw std::runtime_error("Missing login or pass");
    }

    auto& user = m_userRepo.authenticate(login, pass);

    if (!user) {
        throw std::runtime_error("Bad credentials");
    }

    if (user->is_deleted) throw std::runtime_error("User is banned!");

    return {
    {"status", "ok"},
    {"user_id", std::to_string(user->id)},
    {"username", user->username}
    };
}

nlohmann::json RegisterHandler::execute(std::optional<UserID> userID, const nlohmann::json& req)
{
    auto login = req.value("login", "");
    auto pass = req.value("pass", "");
    auto username = req.value("username", "");

    if (login.empty() || pass.empty() || username.empty()) {
        throw std::runtime_error("Missing login or pass or username");
    }

    m_userRepo.registerUser(login, username, hashPassword(pass));

    return {
    {"status", "ok"}
    };
}

nlohmann::json FetchUsersHandler::execute(std::optional<UserID> userID, const nlohmann::json& req)
{

    UserList user_list = m_userRepo.getAllUsersExcept(userID.value());
    
    return {
    {"cmd", "user_list"}, {"users", user_list}
    };
}

nlohmann::json SendMessageHandler::execute(std::optional<UserID> userID, const nlohmann::json& req)
{
    auto content = req.value("content", "");
    auto chat_id = req.value("chat_id", uint64_t{0});

    if (content.empty()) throw std::runtime_error("Trying to send empty message, huh?");
    if (chat_id == 0) throw std::runtime_error("Cannot send message to nowhere");


    auto message = m_messageRepo.saveMessage(chat_id, userID.value(), content);

    return { {"cmd","new_message"},{"user_id",userID.value()}, { "message", message } };
}

nlohmann::json FetchMessageHistoryHandler::execute(std::optional<UserID> userID, const nlohmann::json& req)
{
    auto chat_id = req.value("chat_id", uint64_t{0});

    if (chat_id == 0) throw std::runtime_error("Cannot fetch messages from nowhere");


    auto messageHistory = m_messageRepo.fetchHistory(chat_id);

    return { {"cmd","message_history"},{"message_history",messageHistory}};
}

nlohmann::json CreateChatHandler::execute(std::optional<UserID> userID, const nlohmann::json& req)
{
    auto user_ids = req["user_ids"].get<std::vector<uint64_t>>();
    auto type = req.value("type", "");
    auto name = req.value("name", "");

    m_chatRepo.createChat(user_ids, type, name);
    return { {"cmd","new_chat"}, { "user_ids",user_ids } };
}

nlohmann::json FetchChatsHandler::execute(std::optional<UserID> userID, const nlohmann::json& req)
{
    auto chat_list = m_chatRepo.getChatPreviewsForUser(userID.value());

    return {{"cmd","chat_list"},{"chats",chat_list}};
}

nlohmann::json FetchChatHandler::execute(std::optional<UserID> userID, const nlohmann::json& req)
{
    auto& chat = m_chatRepo.getChatForUser(userID.value(), req.value("chat_id",uint64_t{}));

    return { {"cmd","chat"},{"chat",chat} };
}

nlohmann::json AdminFetchAllInfo::execute(std::optional<UserID> userID, const nlohmann::json& req)
{
    return { {"chats",m_adminRepo.fetchAllChats()},{"users",m_adminRepo.fetchAllUsers()},{"cmd", "full_info"}};
}

nlohmann::json AdminFetchChat::execute(std::optional<UserID> userID, const nlohmann::json& req)
{
    auto& chat = m_adminRepo.fetchChat(req["chat_id"].get<uint64_t>());
    auto& history = m_adminRepo.fetchHistory(req["chat_id"].get<uint64_t>());

    return { {"chat",chat}, {"cmd","chat"}, {"message_history",history} };
}

nlohmann::json AdminBanhammer::execute(std::optional<UserID> userID, const nlohmann::json& req)
{
    m_adminRepo.BanHammer(req["user_id"].get<uint64_t>());

    return { {"cmd","user_status_changed"} };
}