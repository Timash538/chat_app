#include "server/UserRepository.h"
#include "Models.h"
#include "server/PasswordHash.h"
#include <stdexcept>

std::optional<UserFull> UserRepository::authenticate(
    const std::string& login,
    const std::string& password_hash
) {
    auto result = m_db.query(
        "SELECT id, login, username, password_hash "
        "FROM users WHERE login = $1",
        { login }
    );

    if (result.empty()) {
        return std::nullopt;
    }

    const auto& row = result[0];
    std::string stored_hash = row[3];

    if (!verifyPassword(password_hash, stored_hash)) {
        return std::nullopt;
    }

    return UserFull{
        std::stoull(row[0]),  // id
        row[1],             // login
        row[2]             // username
    };
}

void UserRepository::registerUser(
    const std::string& login,
    const std::string& username,
    const std::string& password_hash
) {
    try {
        auto result = m_db.query(
            "INSERT INTO users (login, username, password_hash) "
            "VALUES ($1, $2, $3)",
            { login, username, password_hash }
        );
        return;
    }
    catch (const DatabaseError& e) {
        if (std::string(e.what()).find("unique constraint") != std::string::npos) {
            throw std::runtime_error("User with this login already exists");

        }
    }
}

std::optional<UserFull> UserRepository::findById(uint64_t id) {
    auto result = m_db.query(
        "SELECT id, login, username "
        "FROM users WHERE id = $1",
        { std::to_string(id) }
    );

    if (result.empty()) return std::nullopt;

    const auto& row = result[0];
    return UserFull{
        std::stoull(row[0]),
        row[1],
        row[2]
    };
}

std::optional<UserPreview> UserRepository::findPreviewById(uint64_t id) {
    auto result = m_db.query(
        "SELECT id, username FROM users WHERE id = $1",
        { std::to_string(id) }
    );

    if (result.empty()) return std::nullopt;

    const auto& row = result[0];
    return UserPreview{
        std::stoull(row[0]),
        row[1]
    };
}

UserList UserRepository::getAllUsers() {
    auto result = m_db.query(
        "SELECT id, username FROM users ORDER BY username", {}
    );
    UserList user_list;
    user_list.users.reserve(result.size());
    for (const auto& row : result) {
        user_list.users.push_back({
            std::stoull(row[0]),
            row[1]
            });
    }
    return user_list;
}

UserList UserRepository::getAllUsersExcept(uint64_t id)
{
    auto result = m_db.query(
        "SELECT id, username FROM users WHERE id != $1 ORDER BY username", {std::to_string(id)}
    );
    UserList user_list;
    user_list.users.reserve(result.size());
    for (const auto& row : result) {
        user_list.users.push_back({
            std::stoull(row[0]),
            row[1]
            });
    }
    return user_list;
}

int UserRepository::findByLogin(const std::string& login)
{
    auto result = m_db.query(
        "SELECT id FROM users WHERE login = $1", {login}
    );

    if (result.empty()) return 0;

    return std::stoi(result[0][0]);
}
