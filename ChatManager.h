#pragma once
#include <vector>
#include "Message.h"
#include "UserManager.h"

enum ChatType {FORALL, GROUP, PRIVATE};


class ChatManager
{
	struct Chat;
	struct UserToChatLink;

public:
	ChatManager() = default;
	ChatManager(ChatManager&& other) noexcept;
	ChatManager(const ChatManager& other);
	ChatManager& operator=(ChatManager&& other) noexcept;
	ChatManager& operator=(const ChatManager& other);
	void addNewChat(const ChatType& type, unsigned int userID);
	void addNewChat(const ChatType& type);
	void addUserToChat(int userID, int chatID);
	void addUserToLastChat(int userID);
	bool findUserToChatLink(int userID, int& index);
	void updateUserPtrToLastMsg(int userID, int chatID);
	void showAllUserChats(int userID, const UserManager& uM);
	int countUnreadMsgs(int userID, int chatID);
	const std::vector<Chat>& getChats();
	Chat& getChat(unsigned int chatID);
	const std::vector<UserToChatLink>& getLinks();
	void showMessages(unsigned int chatID);
	void showAllLinks(const UserManager& uM);
	std::vector<int> getUsersIDFromChat(int chatID);

private:

	struct Chat
	{
		ChatType _type;
		std::vector<Message> _messages;
		std::vector<int> _users;

		Chat(ChatType type);
		Chat(const Chat& chat);
		Chat(Chat&& chat) noexcept;
		Chat& operator=(const Chat& chat);
		Chat& operator=(Chat&& chat) noexcept;
		Chat() = default;
	};

	struct UserToChatLink
	{
		int _userID;
		std::vector<int> _chats;
		std::vector<int> _userPtrToLastMsg;

		UserToChatLink(int userID, int chatID);
		UserToChatLink() = default;
	};

	std::vector<Chat> _chats;
	std::vector<UserToChatLink> _links;
};