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

void	connDetect(ServerSocket &soc)
{
}

void	serverTerm(ServerSocket &soc)
{
	while (!cin.eof())
	{
		cout << "[]>"; cout.flush();
		string s;
		std::getline(cin, s);
		if (s == "exit") break;
	}
}

int	main()
{
	ServerDB::get_instance();
	ServerSocket soc;
	while (true)
	{
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
	}

	thread	t(std::bind(connDetect, soc));
	thread	m(std::bind(serverTerm, soc));
	m.join();
	t.terminate();
	return 0;
}
