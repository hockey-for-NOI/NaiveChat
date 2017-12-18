#include "ClientSocket.h"
#include <iostream>
#include <string>

using	std::cin;
using	std::cout;
using	std::endl;
using	std::string;

using	NaiveChat::ClientSocket;

int	main()
{
	while (!cin.eof())
	{
		ClientSocket soc;
		cout << "Welcome to NaiveChat Client!" << endl;
		cout << "Server IP: "; cout.flush();
		string host;
		std::getline(cin, host);
		cout << "Server Port: "; cout.flush();
		int port;
		cin >> port;
		std::getline(cin, host);
		cout << "Connecting..."; cout.flush();
		if (!soc.conn(host.c_str(), port))
		{
			cout << "Failed." << endl;
			continue;
		}	else	cout << "Done." << endl;

		while (!cin.eof())
		{
			cout << "[]>"; cout.flush();
			string s;
			std::getline(cin, s);
			if (s == "exit") break;
			if (!soc.senddata(s.c_str(), s.length()))
				cout << "Send Failed. Input exit to restart or EOF to end." << endl;
		}
	}
	return 0;
}
