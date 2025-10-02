#pragma once
#include <string>

class User 
{
public:

	User(std::string login, std::string nickname, std::string password) : 
		_login(login), _nickname(nickname), _password(password) {}
	User() = default;
	User(const User& other) :
		_login(other._login), _nickname(other._nickname), _password(other._password) {
	}
	User(User&& other) noexcept;
	User& operator=(User&& other) noexcept;
	User& operator=(const User& other);
	std::string getNickname() const;
	std::string getLogin() const;
	std::string getPassword() const;
	void setNickname(const std::string& nickname);
	void setLogin(const std::string& login);
	void setPassword(const std::string& password);
	bool checkPassword(const std::string& password) const;
	bool operator==(const User &b);

private:

	std::string _login;
	std::string _nickname;
	std::string _password;

};