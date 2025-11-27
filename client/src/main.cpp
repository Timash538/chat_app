#include <iostream>
#include <memory>
#include <Message.h>
#include <User.h>
#include <Client.h>
#include <thread>

using namespace std;


int main()
{

	try
	{
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);

		system("cls");

		Client c;
		c.connect("127.0.0.1", 7777);
		while (true)
		{
			string j;
			std::cin >> j;
			std::cout << c.sendRequest(j + '\n');
			if (j == "quit") break;
		}
		
	}
	catch (exception& e)
	{
		cout << e.what();
	}
	return 0;
}