#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct LoginRequest {
    std::string login;
    std::string password;

    static LoginRequest from_json(const nlohmann::json& j) {
        return {
            j.value("login", ""),
            j.value("password", "")
        };
    }
};

struct RegisterRequest {
    std::string login;
    std::string username;
    std::string password;

    static RegisterRequest from_json(const nlohmann::json& j) {
        return {
            j.value("login", ""),
            j.value("username", ""),
            j.value("password", "")
        };
    }
};

struct AuthResponse {
    bool success = false;
    std::string error;
    int user_id = 0;
    std::string username;

    operator nlohmann::json() const {
        if (!success) {
            return { {"success", false}, {"error", error} };
        }
        return {
            {"success", true},
            {"user_id", user_id},
            {"username", username}
        };
    }
};

struct UserPreview {
    uint64_t id = 0;
    std::string username;
    bool is_deleted = false;

    friend void to_json(nlohmann::json& j, const UserPreview& u) {
        j = { {"id", u.id}, {"username", u.username}, {"is_deleted", u.is_deleted} };
    }
    friend void from_json(const nlohmann::json& j, UserPreview& u) {
        u.id = j.value("id", 0);
        u.username = j.value("username", "");
        u.is_deleted = j.value("is_deleted", bool{false});
    }
};

struct UserFull {
    uint64_t id = 0;
    std::string login;
    std::string username;
    bool is_deleted = false;

    friend void to_json(nlohmann::json& j, const UserFull& u) {
        j = {
            {"id", u.id},
            {"login", u.login},
            {"username", u.username},
            {"is_deleted", u.is_deleted}
        };
    }
    friend void from_json(const nlohmann::json& j, UserFull& u) {
        u.id = j.value("id", 0);
        u.login = j.value("login", "");
        u.username = j.value("username", "");
        u.is_deleted = j.value("is_deleted", bool{false});
    }
};

struct UserList {
    std::vector<UserPreview> users;

    friend void to_json(nlohmann::json& j, const UserList& ul) {
        j = nlohmann::json::array();
        for (const auto& u : ul.users) j.push_back(u);
    }
    friend void from_json(const nlohmann::json& j, UserList& ul) {
        ul.users.clear();
        for (const auto& item : j) {
            ul.users.push_back(item.get<UserPreview>());
        }
    }
};

struct ChatPreview {
    uint64_t id = 0;
    std::string name;
    std::string type;

    friend void to_json(nlohmann::json& j, const ChatPreview& c) {
        j = {
            {"id", c.id},
            {"name", c.name},
            {"type", c.type}
        };
    }
    friend void from_json(const nlohmann::json& j, ChatPreview& c) {
        c.id = j.value("id", 0);
        c.name = j.value("name", "");
        c.type = j.value("type", "");
    }
};

struct ChatFull {
    uint64_t id = 0;
    std::string name;
    std::vector<UserPreview> users;

    friend void to_json(nlohmann::json& j, const ChatFull& c) {
        j = {
            {"id", c.id},
            {"name", c.name},
            {"users", c.users}
        };
    }
    friend void from_json(const nlohmann::json& j, ChatFull& c) {
        c.id = j.value("id", 0);
        c.name = j.value("name", "");
        c.users = j.value("users", std::vector<UserPreview>{});
    }
};

struct ChatList {
    std::vector<ChatPreview> chats;

    friend void to_json(nlohmann::json& j, const ChatList& cl) {
        j = nlohmann::json::array();
        for (const auto& c : cl.chats) j.push_back(c);
    }
    friend void from_json(const nlohmann::json& j, ChatList& cl) {
        cl.chats.clear();
        for (const auto& item : j) {
            cl.chats.push_back(item.get<ChatPreview>());
        }
    }
};

struct Message {
    uint64_t id = 0;
    uint64_t chat_id = 0;
    uint64_t sender_id = 0;
    std::string content;
    std::string created_at;

    friend void to_json(nlohmann::json& j, const Message& m) {
        j = {
            {"id", m.id},
            {"chat_id", m.chat_id},
            {"sender_id", m.sender_id},
            {"content", m.content},
            {"created_at", m.created_at}
        };
    }
    friend void from_json(const nlohmann::json& j, Message& m) {
        m.id = j.value("id", 0);
        m.chat_id = j.value("chat_id", 0);
        m.sender_id = j.value("sender_id", 0);
        m.content = j.value("content", "");
        m.created_at = j.value("created_at", "");
    }
};

struct MessageHistory {
    uint64_t chat_id = 0;
    std::vector<Message> messages;

    friend void to_json(nlohmann::json& j, const MessageHistory& mh) {
        j = {
            {"chat_id", mh.chat_id},
            {"messages", mh.messages}
        };
    }
    friend void from_json(const nlohmann::json& j, MessageHistory& mh) {
        mh.chat_id = j.value("chat_id", 0);
        mh.messages = j.value("messages", std::vector<Message>{});
    }
};

struct SendMessageRequest {
    int chat_id = 0;
    std::string content;

    static SendMessageRequest from_json(const nlohmann::json& j) {
        return {
            j.value("chat_id", 0),
            j.value("content", "")
        };
    }
};

struct UserJoinedEvent {
    int user_id;
    std::string username;

    operator nlohmann::json() const {
        return {
            {"cmd", "user_joined"},
            {"user_id", user_id},
            {"username", username}
        };
    }
};

struct NewMessageEvent {
    int chat_id;
    Message message;

    operator nlohmann::json() const {
        return {
            {"cmd", "new_message"},
            {"chat_id", chat_id},
            {"message", message}
        };
    }
};
