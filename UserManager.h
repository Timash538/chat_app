#pragma once
#include "User.h"
#include <vector>
#include <memory>
#include <string>


class UserManager //класс дл€ управлени€ пользовател€ми
{
public:
	UserManager(); 
	UserManager(UserManager&& other) noexcept;
	UserManager(const UserManager& other);
	UserManager& operator=(UserManager&& other) noexcept; 
	UserManager& operator=(const UserManager& other); 
	~UserManager() = default;
	bool addUser(const User& user); // ƒобавл€ет пользовател€, возвращает true если удалось
	const User& getUser(int id) const; // ѕолучить пользовател€ по ID
	int getCurrentUserID() const; // ¬озвращает ID текущего пользовател€ (который залогинилс€)
	const User& getCurrentUser() const; // ¬озвращает текущего пользовател€ (который залогинилс€)
	bool exists(const std::string&,const std::string&) const; // ¬озвращает true если логин или никнейм совпадают с хот€ бы одним пользователем
	bool findUserByNickname(const std::string& nickname, int& index) const; // Ќайти пользовател€ по нику и достать его ID (index)
	bool findUserByNickname(const std::string& nickname) const; // “о же самое но провер€ет существует ли пользователь с таким ником
	bool findUserByLogin(const std::string& login, int& index) const; // Ќайти пользовател€ по логину и достать его ID (index)
	bool findUserByLogin(const std::string& login) const; // “о же самое но провер€ет существует ли пользователь с таким логином
	bool loginUser(const std::string& login,const std::string& password); // ≈сли логин и пароль сход€тс€ - найденный по логину пользователь заноситс€ в currentUser
	void logout(); // ≈сли логин и пароль сход€тс€ - найденный по логину пользователь заноситс€ в currentUser
	void showUsers() const; // ѕоказывает всех пользователей
	void showUsersExcludingCurrent() const; // ѕоказывает всех пользователей исключа€ залогиненного
	void showUsersExcluding(const std::vector<int>& ids) const; // ѕоказывает всех пользователей исключа€ залогиненного
	int getCount() const; // ¬озвращает кол-во пользователей
private:
	std::vector<User> _users;
	int _currentUserID;
};