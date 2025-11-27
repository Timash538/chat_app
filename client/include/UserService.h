#pragma once

#include <IUserService.h>
#include <Client.h>

class UserService : public IUserService
{
private:
	Client* _client;
public:
	explicit UserService(Client* client) : _client(client) {}

	std::shared_ptr<User> login(const std::string& login, const std::string& password);
	bool registerUser(const std::string& login, const std::string& nickname, const std::string& password);
};