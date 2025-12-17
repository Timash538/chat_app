#include <server/AdminRepository.h>
#include <iostream>


std::vector<UserFull> AdminRepository::fetchAllUsers()
{
    try {
        auto result = m_db.query(R"(
        SELECT u.id, u.login, u.username, u.is_deleted FROM users u)", {});

        if (result.empty())
            return {};

        std::vector<UserFull> users;
        users.reserve(result.size());
        for (auto& row : result)
        {
            users.push_back(UserFull{ std::stoull(row[0]),row[1],row[2],(row[3]=="t")});
        }
        return users;
    }
    catch (const DatabaseError& e)
    {
        throw std::runtime_error("Failed to retrieve chats from DB: " + std::string(e.what()));
    }
}

std::vector<ChatFull> AdminRepository::fetchAllChats()
{
    try {
        auto result = m_db.query(R"(
            SELECT 
                c.id,
                CASE 
                    WHEN c.type = 'direct' THEN 
                        string_agg(u.username, ' - ' ORDER BY u.username)
                    ELSE c.name
                END AS chat_name,
                json_agg(
                    json_build_object(
                        'id', u.id, 
                        'username', u.username, 
                        'is_deleted', u.is_deleted
                    ) ORDER BY u.id
                ) AS users_json
            FROM chats c
            JOIN chatmembers cm ON cm.chat_id = c.id
            JOIN users u ON u.id = cm.user_id
            GROUP BY c.id, c.name, c.type
            ORDER BY c.id
        )", {});

        if (result.empty()) {
            return {};
        }

        std::vector<ChatFull> chats;
        chats.reserve(result.size());

        for (const auto& row : result) {
            ChatFull chat;
            chat.id = std::stoull(row[0]);
            chat.name = row[1];

            // Парсим JSON массив пользователей
            if (!row[2].empty()) {
                auto users_json = nlohmann::json::parse(row[2]);
                for (const auto& user_json : users_json) {
                    UserPreview user;
                    user.id = user_json["id"].get<uint64_t>();
                    user.username = user_json["username"].get<std::string>();
                    chat.users.push_back(user);
                }
            }

            chats.push_back(std::move(chat));
        }

        return chats;
    }
    catch (const DatabaseError& e) {
        throw std::runtime_error(
            "Failed to retrieve chats with users from DB: " + std::string(e.what()));
    }
}
bool AdminRepository::BanHammer(uint64_t user_id)
{
    try {
        auto result = m_db.query(R"(
        UPDATE users u
        SET is_deleted = NOT is_deleted
        WHERE id = $1 RETURNING u.is_deleted AS new_status)", {std::to_string(user_id)});

        return result[0][0] == "t";
    }
    catch (const DatabaseError& e)
    {
        throw std::runtime_error("Failed to retrieve chats from DB: " + std::string(e.what()));
    }
}

ChatFull AdminRepository::fetchChat(const uint64_t& chat_id)
{
    try {
        auto result = m_db.query(R"(
    SELECT 
        c.id,
        CASE 
            WHEN c.type = 'direct' THEN 
                string_agg(u.username, ' - ' ORDER BY u.username)
            ELSE c.name
        END AS chat_name,
        json_agg(json_build_object('id', u.id, 'username', u.username, 'is_deleted', u.is_deleted) ORDER BY u.id) AS users
    FROM chats c
    JOIN chatmembers cm ON cm.chat_id = c.id
    JOIN users u ON u.id = cm.user_id
    WHERE c.id = $1
    GROUP BY c.id, c.name)", { std::to_string(chat_id) });

        if (result.empty()) {
            return ChatFull{};  // Пустой объект
        }

        const auto& row = result[0];

        // Парсим JSON массив пользователей
        ChatFull chat;
        chat.id = std::stoull(row[0]);
        chat.name = row[1];

        // Парсинг users_json (PostgreSQL array of JSON)
        if (!row[2].empty()) {
            auto users_json = nlohmann::json::parse(row[2]);
            for (const auto& user_json : users_json) {
                UserPreview user;
                user.id = user_json["id"].get<uint64_t>();
                user.username = user_json["username"].get<std::string>();
                chat.users.push_back(user);
            }
        }

        return chat;
    }
    catch (const DatabaseError& e) {
        throw std::runtime_error("Failed to retrieve chat from DB: " + std::string(e.what()));
    }
}

MessageHistory AdminRepository::fetchHistory(const uint64_t& chat_id)
{
    auto result = m_db.query(
        "SELECT id, chat_id, sender_id, content, created_at "
        "FROM messages WHERE chat_id = $1 ORDER BY id",
        { std::to_string(chat_id) }
    );

    if (result.empty()) {
        return { chat_id,{} };
    }

    std::vector<Message> messages;
    messages.reserve(result.size());

    for (const auto& row : result)
    {
        messages.push_back({
            std::stoull(row[0]),
            std::stoull(row[1]),
            std::stoull(row[2]),
            row[3],
            row[4]
            });
    }
    return { chat_id, messages };
}