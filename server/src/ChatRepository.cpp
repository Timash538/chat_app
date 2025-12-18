#include "server/ChatRepository.h"


ChatList ChatRepository::getChatPreviewsForUser(const uint64_t& user_id)
{
    try {
        auto result = m_db.query(R"(
            SELECT 
                c.id,
                CASE 
                    WHEN c.type = 'direct' THEN u.username
                    ELSE c.name
                END AS chat_name,
                c.type
            FROM chats c
            JOIN chatmembers cm ON cm.chat_id = c.id AND cm.user_id = $1
            LEFT JOIN chatmembers cm2 ON cm2.chat_id = c.id 
                AND cm2.user_id != $1
                AND c.type = 'direct'
            LEFT JOIN users u ON u.id = cm2.user_id
            ORDER BY c.id DESC
            
	)", { std::to_string(user_id) });

        if (result.empty())
            return {};

        ChatList chats;
        chats.chats.reserve(result.size());
        for (const auto& row : result)
        {
            chats.chats.push_back({
                std::stoull(row[0]),
                row[1],
                row[2]
                });
        }
        return chats;
    }
    catch (const DatabaseError& e)
    {
        throw std::runtime_error("Failed to retrieve chats from DB: " + std::string(e.what()));
    }
}

uint64_t ChatRepository::createChat(
    const std::vector<uint64_t>& user_ids, 
    const std::string& type, 
    const std::optional<std::string>& name)
{ 
    if (user_ids.size() < 2)
    {
        throw std::runtime_error("Chat must have at least 2 members");
    }

    try {
        std::string chat_name = name.value_or("");
        std::string insert_chat_sql;
        std::vector<std::string> insert_chat_params;

        if (type == "direct") {
            if (user_ids.size() != 2) {
                throw std::runtime_error("Direct chat must have exactly 2 members");
            }

            auto existing = m_db.query(
                "SELECT c.id FROM chats c "
                "JOIN chatmembers cm1 ON cm1.chat_id = c.id AND cm1.user_id = $1 "
                "JOIN chatmembers cm2 ON cm2.chat_id = c.id AND cm2.user_id = $2 "
                "WHERE c.type = 'direct'",
                { std::to_string(user_ids[0]), std::to_string(user_ids[1]) }
            );

            if (!existing.empty()) {
                return std::stoull(existing[0][0]);
            }
            insert_chat_sql = "INSERT INTO chats (type) VALUES ($1) RETURNING id";
            insert_chat_params = { type };
        }
        else {
            if (chat_name.empty())
                throw std::runtime_error("Group chat needs a name!");

            insert_chat_sql = "INSERT INTO chats (type, name) VALUES ($1, $2) RETURNING id";
            insert_chat_params = { type, chat_name };
        }

        auto chat_result = m_db.query(insert_chat_sql, insert_chat_params);

        for (uint64_t user_id : user_ids) {
            m_db.query(
                "INSERT INTO chatmembers (chat_id, user_id) VALUES ($1, $2)",
                { chat_result[0][0], std::to_string(user_id)}
            );
        }

        return std::stoull(chat_result[0][0]);
    }
    catch (const DatabaseError& e) {
        throw std::runtime_error("Failed to create chat: " + std::string(e.what()));
    }
}

ChatFull ChatRepository::getChatForUser(const uint64_t& user_id, const uint64_t& chat_id)
{
    try {
        auto result = m_db.query(R"(
    SELECT 
        c.id,
        CASE 
            WHEN c.type = 'direct' THEN (
                SELECT u.username 
                FROM users u 
                JOIN chatmembers cm ON cm.user_id = u.id 
                WHERE cm.chat_id = c.id AND u.id != $2
                LIMIT 1
            )
            ELSE c.name
        END AS chat_name,
        json_agg(json_build_object('id', u.id, 'username', u.username, 'is_deleted', u.is_deleted)) AS users
    FROM chats c
    JOIN chatmembers cm ON cm.chat_id = c.id
    JOIN users u ON u.id = cm.user_id
    WHERE c.id = $1
    GROUP BY c.id, chat_name
    )", { std::to_string(chat_id), std::to_string(user_id) });

        if (result.empty()) {
            return ChatFull{};
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
                user.username = (user_json["is_deleted"].get<bool>()) ? "[DELETED]" : user_json["username"].get<std::string>();
                chat.users.push_back(user);
            }
        }

        return chat;
    }
    catch (const DatabaseError& e) {
        throw std::runtime_error("Failed to retrieve chat from DB: " + std::string(e.what()));
    }
}

std::vector<uint64_t> ChatRepository::getUsersFromChat(const uint64_t& chat_id)
{
    try 
    {
        auto result = m_db.query(R"(
        SELECT u.id
        FROM users u
        JOIN chatmembers cm ON cm.user_id=u.id AND cm.chat_id = $1
        )", { std::to_string(chat_id) });

        if (result.empty()) {
            return {};
        }

        std::vector<uint64_t> ids;
        for (const auto& row : result)
        {
            ids.push_back(std::stoull(row[0]));
        }

        return ids;
    }
    catch (const DatabaseError& e) {
        throw std::runtime_error("Failed to retrieve chat from DB: " + std::string(e.what()));
    }
}