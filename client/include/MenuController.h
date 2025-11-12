#pragma once
#include <iostream>
#include "UserManager.h"
#include "ChatManager.h"
#include <memory>

class MenuController
{
public:
	MenuController(std::unique_ptr<UserManager> uM, std::unique_ptr<ChatManager> cM);
	void startMenu();
	void userRegistration();
	void login();
	void userMenu();
	void newChatMenu();
	void chatMenu(unsigned int id);
	void addUserToChatMenu(unsigned int id);
	
	UserManager& getUM();
	ChatManager& getCM();

private:
	int getRealChatID(unsigned int id);

	std::unique_ptr<UserManager> _uM;
	std::unique_ptr<ChatManager> _cM;
};