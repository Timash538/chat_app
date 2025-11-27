#include <UserManager.h>
#include <iostream>

UserManager::UserManager() : _users(), _currentUserID(-1) {}

UserManager::UserManager(UserManager&& other) noexcept : _users(std::move(other._users)), _currentUserID(other._currentUserID)
{
}

UserManager::UserManager(const UserManager& other) : _users(other._users), _currentUserID(other._currentUserID)
{
}

UserManager& UserManager::operator=(UserManager&& other) noexcept
{
	_users = std::move(other._users);
	_currentUserID = other._currentUserID;
	return *this;
}

UserManager& UserManager::operator=(const UserManager& other) 
{
	_users = other._users;
	_currentUserID = other._currentUserID;
	return *this;
}

bool UserManager::addUser(const User& user)
{
	if (!exists(user.getNickname(), user.getLogin()))
	{
		_users.push_back(user);
		std::cout << "Пользователь " << user.getNickname() << " успешно зарегистрирован!" << std::endl;
		return true;
	}
	std::cout << "Пользователь с таким ником или логином уже зарегистрирован!" << std::endl;
	return false;
}

const User& UserManager::getUser(int id) const
{
	return _users.at(id);
}

int UserManager::getCurrentUserID() const
{
	return _currentUserID;
}

const User& UserManager::getCurrentUser() const
{
	return getUser(_currentUserID);
}

bool UserManager::exists(const std::string& nickname,const std::string& login) const
{
	for (int i = 0; i < _users.size(); i++)
	{
		if (_users.at(i).getNickname() == nickname || _users.at(i).getLogin() == login)
		{
			return true;
		}
	}
	return false;
}

bool UserManager::findUserByNickname(const std::string& nickname, int& index) const
{
	for (int i = 0; i < _users.size(); i++)
	{
		if (_users.at(i).getNickname() == nickname)
		{
			index = i;
			return true;
		}
	}
	return false;
}

bool UserManager::findUserByNickname(const std::string& nickname) const
{
	int dummyIndex;
	return findUserByNickname(nickname, dummyIndex);
}

bool UserManager::findUserByLogin(const std::string& login, int& index) const
{
	for (int i = 0; i < _users.size(); i++)
	{
		if (_users.at(i).getLogin() == login)
		{
			index = i;
			return true;
		}
	}
	return false;
}

bool UserManager::findUserByLogin(const std::string& login) const
{
	for (int i = 0; i < _users.size(); i++)
	{
		if (_users.at(i).getLogin() == login)
		{
			return true;
		}
	}
	return false;
}

bool UserManager::loginUser(const std::string& login,const std::string& password)
{
	int index = -1;

	if (findUserByLogin(login, index))
		if (_users.at(index).checkPassword(password))
		{
			std::cout << "Успешный вход в чат! Добро пожаловать, " << _users.at(index).getNickname() << "!" << std::endl;
			_currentUserID = index;
			return true;
		}

	std::cout << "Неверный логин или пароль!" << std::endl;
	return false;

}

void UserManager::logout()
{
	_currentUserID = -1;
}

void UserManager::showUsers() const
{
	for (int i = 0; i < _users.size(); i++)
	{
		std::cout << "ID: " << i << " " << " Никнейм: " << _users.at(i).getNickname() << std::endl;
	}
}

void UserManager::showUsersExcludingCurrent() const
{
	for (int i = 0; i < _users.size(); i++)
	{
		if (i != _currentUserID)
		std::cout << "ID: " << i << " " << " Никнейм: " << _users.at(i).getNickname() << std::endl;
	}
}

void UserManager::showUsersExcluding(const std::vector<int>& ids) const
{
	for (int i = 0; i < _users.size(); i++)
	{
		bool f = false;
		for (int j = 0; j < ids.size(); j++)
			if (i == ids[j])
			{
				f = true;
				break;
			}
		if (!f) std::cout << "ID: " << i << " " << " Никнейм: " << _users.at(i).getNickname() << std::endl;
	}
}

int UserManager::getCount() const
{
	return _users.size();
}
