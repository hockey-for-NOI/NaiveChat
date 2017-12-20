#include "ServerSocket.h"
#include "ServerDB.h"
#include "Pack.h"
#include "Debug.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <exception>
#include <thread>
#include <cstring>

using	std::cin;
using	std::cout;
using	std::endl;
using	std::vector;
using	std::thread;
using	std::string;

using	NaiveChat::ServerSocket;
using	NaiveChat::ServerDB;
using	NaiveChat::User;
using	NaiveChat::Pack;

void	userConn(std::shared_ptr<User> usr, std::shared_ptr<ServerSocket> soc)
{
	if (usr->status) usr->status->closeconn();
	usr->setsoc(soc);
	soc->senddata("succeeded", 9);
	Pack	p;
	try
	{
		while (soc->isvalid())
		{
			if (!soc->recvdata(&p)) break;
			switch (p.op)
			{
				case Pack::OP_UPDATE:
				break;
				case Pack::OP_LOGOUT:
					soc->closeconn();
				break;
				case Pack::OP_CPWD:
					usr->cpwd(p.cpwd);
				break;
			}
		}
	}
	catch (std::exception &e) {}
	soc->closeconn();
}

void	connDetect(std::shared_ptr<ServerSocket> soc)
{
	char	buffer[ServerSocket::MAX_SIZE + 1];
	std::vector <std::thread> threads;
	while (true)
	{
		auto new_soc = std::make_shared<ServerSocket>();
		if (soc->acceptconn(*new_soc))
		{
			memset(buffer, 0, sizeof(buffer));
			new_soc->recvdata(buffer);
			std::string op = buffer;
			if (op != "regist" && op != "login") continue;
			memset(buffer, 0, sizeof(buffer));
			new_soc->recvdata(buffer);
			std::string name = buffer;
			memset(buffer, 0, sizeof(buffer));
			new_soc->recvdata(buffer);
			std::string sp1 = buffer;
			if (op == "regist")
			{
				auto tmp = ServerDB::get_instance().regist(name, sp1);
				if (tmp) threads.emplace_back(std::bind(userConn, tmp, new_soc));
			}
			else if (op == "login")
			{
				auto tmp = ServerDB::get_instance().login(name, sp1);
				if (tmp) threads.emplace_back(std::bind(userConn, tmp, new_soc));
			}
		}
	}
}

void	serverTerm(std::shared_ptr<ServerSocket> soc)
{
	while (!cin.eof())
	{
		cout << "[]>"; cout.flush();
		string s;
		std::getline(cin, s);
		if (s == "exit") break;
		if (s == "ls")
		{
			for (auto &&i: ServerDB::get_instance().listuser())
				cout << i.first << "(" << (i.second ? "Online" : "Offline") << ")" << endl;
			continue;
		}
		if (s != "help") cout << "Unrecognized command." << endl;
		cout << "ls\tShow all users." << endl;
		cout << "help\tShow this message." << endl;
		cout << "exit\tExit server." << endl;
	}
}

int	main()
{
	ServerDB::get_instance();
	auto soc = std::make_shared<ServerSocket>();
	while (true)
	{
		soc = std::make_shared<ServerSocket>();
		cout << "Welcome to NaiveChat Server!" << endl;
		cout << "Port: "; cout.flush();
		int port;
		cin >> port;
		std::string s;
		std::getline(cin, s);
		cout << "Binding..."; cout.flush();
		if (!soc->bindport(port))
		{
			cout << "Failed." << endl;
			continue;
		}	else	cout << "Done." << endl;
		cout << "Listeninging..."; cout.flush();
		if (!soc->listento())
		{
			cout << "Failed." << endl;
			continue;
		}	else	cout << "Done." << endl;
		break;
	}

	thread	t(std::bind(connDetect, soc));
	thread	m(std::bind(serverTerm, soc));
	if (m.joinable()) m.join();
	std::terminate();
	return 0;
}
