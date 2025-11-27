#pragma once

class IChatService
{
public:
	enum ChatType { FORALL, GROUP, PRIVATE };

	virtual ~IChatService() = default;

	virtual bool openChat(ChatType);
	virtual bool addUserToChat();
};