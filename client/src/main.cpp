#include <iostream>
#include <memory>
#include <client/Client.h>
#include <thread>
#include <nlohmann/json.hpp>

using namespace std;


int main()
{

	try
	{
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);

		system("cls");

		Client c;
		c.connect("localhost", 9955);
		while (true)
		{
			string j;
			std::cin >> j;
			//std::cout << c.sendRequest(j + '\n');
			if (j == "quit") break;
			nlohmann::json req;
			req["cmd"] = "ping";
			std::cout << c.sendRequest(req.dump() + "\n");
		}
		
	}
	catch (exception& e)
	{
		cout << e.what();
	}
	return 0;
}