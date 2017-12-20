#include "ServerSocket.h"
#include "ServerDB.h"
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

void	userConn(std::shared_ptr<User> usr, std::shared_ptr<ServerSocket> soc)
{
	if (usr->status) usr->status->closeconn();
	usr->status = soc;
	soc->senddata("succeeded", 9);
	char	buffer[ServerSocket::MAX_SIZE + 1];
	try
	{
		while (soc->isvalid())
		{
			soc->recvdata(buffer);
		}
	}
	catch (std::exception &e) {}
	usr->status->closeconn();
	usr->status = nullptr;
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
			for (auto &&i: ServerDB::get_instance().listuser()) cout << i << endl;
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
