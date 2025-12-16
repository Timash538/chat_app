#pragma once

#include <optional>
#include <Models.h>
#include <server/Database.h>
#include <string>

class UserRepository
{
public:
	explicit UserRepository(Database& db) : m_db(db) {}
	std::optional<UserFull> authenticate(const std::string& login, const std::string& password_hash);
	void registerUser(const std::string& login, const std::string& username, const std::string& password_hash);
	int findByLogin(const std::string& login);
    std::optional<UserFull> findById(uint64_t id);
    std::optional<UserPreview> findPreviewById(uint64_t id);
	UserList getAllUsers();
	UserList getAllUsersExcept(uint64_t id);
private:
	Database& m_db;
};
