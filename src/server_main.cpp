#include "ServerSocket.h"
#include "Debug.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <thread>

using	std::cin;
using	std::cout;
using	std::endl;
using	std::vector;
using	std::thread;
using	std::string;

using	NaiveChat::ServerSocket;

int	main()
{
	vector <thread> threads;
	while (true)
	{
		ServerSocket soc;
		cout << "Welcome to NaiveChat Server!" << endl;
		cout << "Port: "; cout.flush();
		int port;
		cin >> port;
		cout << "Binding..."; cout.flush();
		if (!soc.bindport(port))
		{
			cout << "Failed." << endl;
			continue;
		}	else	cout << "Done." << endl;
		cout << "Listeninging..."; cout.flush();
		if (!soc.listento())
		{
			cout << "Failed." << endl;
			continue;
		}	else	cout << "Done." << endl;

		while (true)
		{
			auto user_soc = std::make_shared<ServerSocket>();
			if (soc.acceptconn(*user_soc))
				threads.emplace_back(std::bind(NaiveChat::debug, user_soc));
		}
	}
	return 0;
}
