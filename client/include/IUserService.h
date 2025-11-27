#pragma once
#include <memory>
#include <User.h>
#include <vector>

class IUserService
{
public:
	virtual ~IUserService() = default;
	virtual std::shared_ptr<User> login(const std::string& login, const std::string& password);
	virtual bool registerUser(const std::string& login, const std::string& nickname, const std::string& password);
	virtual std::vector<User> getUsers();

};