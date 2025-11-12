#include <ChatManager.h>
#include <iostream>



ChatManager::ChatManager(ChatManager&& other) noexcept
{
	_chats = std::move(other._chats);
	_links = std::move(other._links);
}

ChatManager::ChatManager(const ChatManager& other)
{
	_chats = other._chats;
	_links = other._links;
}

ChatManager& ChatManager::operator=(ChatManager&& other) noexcept
{
	_chats = std::move(other._chats);
	_links = std::move(other._links);
	return *this;
}

ChatManager& ChatManager::operator=(const ChatManager& other)
{
	_chats = other._chats;
	_links = other._links;
	return *this;
}

void ChatManager::addNewChat(const ChatType &type, unsigned int userID)
{
	_chats.push_back(Chat(type));
	addUserToLastChat(userID);
}

void ChatManager::addNewChat(const ChatType& type)
{
	_chats.push_back(Chat(type));
}

void ChatManager::addUserToChat(int userID, int chatID)
{
	_chats.at(chatID)._users.push_back(std::move(userID));
	int idx = -1;
	if (findUserToChatLink(userID, idx))
	{
		_links.at(idx)._chats.push_back(std::move(chatID));
		_links.at(idx)._userPtrToLastMsg.push_back(0);
		return;
	}
	_links.push_back(UserToChatLink(userID, chatID));
	_links.at(_links.size() - 1)._userPtrToLastMsg.push_back(0);
}

void ChatManager::addUserToLastChat(int userID)
{
	unsigned int lastChatID = _chats.size() - 1;
	_chats.at(lastChatID)._users.push_back(std::move(userID));
	int idx = -1;
	if (findUserToChatLink(userID, idx))
	{
		_links.at(idx)._chats.push_back(lastChatID);
		_links.at(idx)._userPtrToLastMsg.push_back(0);
		return;
	}
	_links.push_back(UserToChatLink(userID, _chats.size()-1));
	_links.at(_links.size()-1)._userPtrToLastMsg.push_back(0);
}

bool ChatManager::findUserToChatLink(int userID, int& idx)
{
	for (int i = 0; i < _links.size(); i++)
	{
		if (_links.at(i)._userID == userID)
		{
			idx = i;
			return true;
		}
	}
	return false;
}

void ChatManager::updateUserPtrToLastMsg(int userID, int chatID)
{
	int idx = -1;
	findUserToChatLink(userID, idx);
	if (idx == -1) return;
	_links.at(idx)._userPtrToLastMsg.at(chatID) = _chats.at(_links.at(idx)._chats.at(chatID))._messages.size();
}

void ChatManager::showAllUserChats(int userID, const UserManager& uM)
{
	int idx = -1;
	findUserToChatLink(userID, idx);
	if (idx == -1) return;

	if (_links.at(idx)._chats.empty())
	{
		std::cout << "Нет чатов, попробуйте создать новый." << std::endl;
		return;
	}
	std::cout << "Все доступные вам чаты: " << std::endl;
	
	for (int i = 0; i < _links.at(idx)._chats.size(); i++)
	{
		unsigned int id = _links.at(idx)._chats[i];
		std::vector<int>* users = &_chats.at(id)._users;
		if (_chats.at(id)._type == FORALL)
		{
			std::cout << "\tID: " << i << " Общий чат. " << std::endl;
			continue;
		}
		else if (_chats.at(id)._type == GROUP)
		{
			std::cout << "\tID: " << i << " участники чата: ";
			for (int j = 0; j < users->size(); j++)
			{
				std::cout << uM.getUser(users->at(j)).getNickname() << ((j == users->size() - 1) ? ". " : ", ");
			}
		}
		else
		{
			std::cout << "\tID: " << i << " личный чат с " << ((users->at(0) == userID) ? uM.getUser(users->at(1)).getNickname() : uM.getUser(users->at(0)).getNickname()) << ". ";
		}
		if (countUnreadMsgs(userID, i) > 0)
		{
			int chatID = _links.at(idx)._chats[i];
			std::cout << "(Непрочитанных сообщений: " << countUnreadMsgs(userID, i) << ")";
			std::cout << "\n";
		}
		else std::cout << "\n";
	}
}

int ChatManager::countUnreadMsgs(int userID, int id)
{
	int idx = -1;
	findUserToChatLink(userID, idx);
	return (_chats.at(_links.at(idx)._chats.at(id))._messages.size()) - _links.at(idx)._userPtrToLastMsg.at(id);
}

const std::vector<ChatManager::Chat>& ChatManager::getChats()
{
	return _chats;
}

ChatManager::Chat& ChatManager::getChat(unsigned int chatID)
{
	return _chats.at(chatID);
}

const std::vector<ChatManager::UserToChatLink>& ChatManager::getLinks()
{
	return _links;
}

void ChatManager::showMessages(unsigned int chatID)
{
	std::vector<Message>* messages = &_chats.at(chatID)._messages;
	for (unsigned int i = 0; i < messages->size(); i++)
	{
		std::cout << messages->at(i).getSendFrom().getNickname() << ": " << messages->at(i).getMessage() << "\n";
	}
}

void ChatManager::showAllLinks(const UserManager& uM)
{
	std::cout << "\n All links:\n";
	for (int i = 0; i < _links.size(); i++)
	{
		std::cout << "ID: " << i << "\n";
		std::cout << "User: " << uM.getUser(_links[i]._userID).getNickname() << "\n";
		std::cout << "Chats: \n";
		for (int j = 0; j < _links[i]._chats.size(); j++)
		{
			std::cout << "\tID of chat: " << _links[i]._chats[j] << " Users: "; 
			for (int k = 0; k < _chats[_links[i]._chats[j]]._users.size(); k++)
				std::cout << uM.getUser(_chats[_links[i]._chats[j]]._users[k]).getNickname() << " ";
			std::cout << "\n";
			std::cout << "\tMessages count: " << _chats[_links[i]._chats[j]]._messages.size() << "\n";
			std::cout << "\tLast read message: " << _links[i]._userPtrToLastMsg[j] << "\n";
		}
	}
}

std::vector<int> ChatManager::getUsersIDFromChat(int chatID)
{
	return std::vector<int>(_chats.at(chatID)._users);
}

ChatManager::UserToChatLink::UserToChatLink(int userID, int chatID)
{
	_userID = userID;
	_chats.push_back(std::move(chatID));
	_userPtrToLastMsg.push_back(0);
}

ChatManager::Chat::Chat(ChatType type)
{
	_type = type;
}

ChatManager::Chat::Chat(const Chat& other)
{
	_type = other._type;
	_messages = other._messages;
	_users = other._users;
}

ChatManager::Chat::Chat(Chat&& other) noexcept
{
	_type = std::move(other._type);
	_messages = std::move(other._messages);
	_users = std::move(other._users);
}

ChatManager::Chat& ChatManager::Chat::operator=(const Chat& other)
{
	_type = other._type;
	_messages = other._messages;
	_users = other._users;
	return *this;
}

ChatManager::Chat& ChatManager::Chat::operator=(Chat&& other) noexcept
{
	_type = std::move(other._type);
	_messages = std::move(other._messages);
	_users = std::move(other._users);
	return *this;
}
