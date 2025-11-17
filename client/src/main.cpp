#include <iostream>
#include <Windows.h>
#include <memory>
#include <Message.h>
#include <User.h>
#include <UserManager.h>
#include <ChatManager.h>
#include <MenuController.h>

using namespace std;


int main()
{
	try
	{
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);

		MenuController mC(make_unique<UserManager>(), make_unique<ChatManager>());

		mC.getUM().addUser(User("admin", "Саша", "123"));
		mC.getUM().addUser(User("qw", "Димон", "qw"));
		mC.getUM().addUser(User("as", "Григорий Петрович", "as"));
		mC.getUM().addUser(User("zx", "NagibatorZZZ", "zx"));
		mC.getUM().addUser(User("1", "Scorpio", "lol"));
		mC.getUM().addUser(User("art", "EvilArthas", "legchayshayadlyavelichayshego123"));

		mC.getCM().addNewChat(ChatType::FORALL);
		for (int i = 0; i < mC.getUM().getCount(); i++) mC.getCM().addUserToLastChat(i);
		
		mC.getCM().addNewChat(ChatType::PRIVATE, 0);
		mC.getCM().addNewChat(ChatType::GROUP, 0);
		mC.getCM().addNewChat(ChatType::GROUP, 0);
		mC.getCM().addNewChat(ChatType::PRIVATE, 0);
		mC.getCM().addNewChat(ChatType::GROUP, 0);
		mC.getCM().addNewChat(ChatType::GROUP, 0);
		mC.getCM().addNewChat(ChatType::GROUP, 0);

		mC.getCM().addUserToChat(3, 4);
		mC.getCM().addUserToChat(1, 2);
		mC.getCM().addUserToChat(2, 2);
		mC.getCM().addUserToChat(4, 2);
		mC.getCM().addUserToChat(4, 3);
		mC.getCM().addUserToChat(3, 2);
		mC.getCM().addUserToChat(4, 1);
		mC.getCM().showAllUserChats(0, mC.getUM());

		system("cls");

		mC.startMenu();
		
	}
	catch (exception& e)
	{
		cout << e.what();
	}
	return 0;
}