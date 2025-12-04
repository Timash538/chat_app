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
    std::string error;      // заполняется при success == false
    int user_id = 0;        // заполняется при success == true
    std::string username;   // заполняется при success == true

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
    int id = 0;
    std::string username;

    friend void to_json(nlohmann::json& j, const UserPreview& u) {
        j = { {"id", u.id}, {"username", u.username} };
    }
    friend void from_json(const nlohmann::json& j, UserPreview& u) {
        u.id = j.value("id", 0);
        u.username = j.value("username", "");
    }
};

struct UserFull {
    int id = 0;
    std::string login;          
    std::string username;       
    std::string created_at;     

    friend void to_json(nlohmann::json& j, const UserFull& u) {
        j = {
            {"id", u.id},
            {"login", u.login},
            {"username", u.username},
            {"created_at", u.created_at}
        };
    }
    friend void from_json(const nlohmann::json& j, UserFull& u) {
        u.id = j.value("id", 0);
        u.login = j.value("login", "");
        u.username = j.value("username", "");
        u.created_at = j.value("created_at", "");
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
    int id = 0;
    std::string name;               
    std::string last_message;       
    std::string last_sender;        
    std::string last_time;          

    friend void to_json(nlohmann::json& j, const ChatPreview& c) {
        j = {
            {"id", c.id},
            {"name", c.name},
            {"last_message", c.last_message},
            {"last_sender", c.last_sender},
            {"last_time", c.last_time}
        };
    }
    friend void from_json(const nlohmann::json& j, ChatPreview& c) {
        c.id = j.value("id", 0);
        c.name = j.value("name", "");
        c.last_message = j.value("last_message", "");
        c.last_sender = j.value("last_sender", "");
        c.last_time = j.value("last_time", "");
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
    int id = 0;
    int sender_id = 0;
    std::string sender_name;
    std::string content;
    std::string timestamp; // ISO8601

    friend void to_json(nlohmann::json& j, const Message& m) {
        j = {
            {"id", m.id},
            {"sender_id", m.sender_id},
            {"sender_name", m.sender_name},
            {"content", m.content},
            {"timestamp", m.timestamp}
        };
    }
    friend void from_json(const nlohmann::json& j, Message& m) {
        m.id = j.value("id", 0);
        m.sender_id = j.value("sender_id", 0);
        m.sender_name = j.value("sender_name", "");
        m.content = j.value("content", "");
        m.timestamp = j.value("timestamp", "");
    }
};

struct MessageHistory {
    int chat_id = 0;
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