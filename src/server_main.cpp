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
#include <cassert>

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
	usr->closeconn();
	usr->setsoc(soc);
	Pack	p;
	p.op = Pack::OP_REPLY;
	p.reply = true;
	try
	{
		soc->sendobj(p);
		while (soc->isvalid())
		{
			if (!soc->recvobj(p)) break;
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
				case Pack::OP_SEARCH:
					p.op = Pack::OP_SEARCHRES;
					p.searchres.len = 0;
					for (auto &&i: ServerDB::get_instance().listuser()) if (i.second) 
					{
						strcpy(p.searchres.name[p.searchres.len++], i.first.c_str());
						if (p.searchres.len == 20)
						{
							p.searchres.hasnext = true;
							if (!soc->sendobj(p)) break;
							p.searchres.len = 0;
						}
					}
					p.searchres.hasnext = false;
					soc->sendobj(p);
				break;
				case Pack::OP_ADD:
				{
					p.op = Pack::OP_REPLY;
					auto u = ServerDB::get_instance().getuser(p.addfriend.name);
					p.reply = u && usr->add(p.addfriend.name, u);
					soc->sendobj(p);
				}
				break;
				case Pack::OP_LISTFRIEND:
				{
					p.op = Pack::OP_LISTFRIENDRES;
					auto &tar = p.listfriendres;
					tar.len = 0;
					for (auto &&i: usr->listfriend()) 
					{
						tar.online[tar.len] = i.second;
						strcpy(tar.name[tar.len++], i.first.c_str());
						if (tar.len == 20)
						{
							tar.hasnext = true;
							if (!soc->sendobj(p)) break;
							tar.len = 0;
						}
					}
					tar.hasnext = false;
					soc->sendobj(p);
				}
				break;
				case Pack::OP_STARTCHAT:
				{
					auto u = ServerDB::get_instance().getuser(p.chat.name);
					usr->chat(u);
					p.op = Pack::OP_REPLY;
					p.reply = (bool)u;
					soc->sendobj(p);
				}
				break;
				case Pack::OP_STOPCHAT:
					usr->chat();
					soc->sendobj(p);
				break;
				case Pack::OP_CHATMSG:
					usr->forward(p);
				break;
			}
		}
	}
	catch (std::exception &e) {}
	soc->closeconn();
}

void	connDetect(std::shared_ptr<ServerSocket> soc)
{
	Pack p;
	std::vector <std::thread> threads;
	while (true)
	{
		auto new_soc = std::make_shared<ServerSocket>();
		if (soc->acceptconn(*new_soc))
		{
			new_soc->recvobj(p);
			if (p.op == Pack::OP_REGIST)
			{
				//cout << "REG" << endl;
				auto tmp = ServerDB::get_instance().regist(p.namesp1.name, p.namesp1.passwd);
				if (tmp) threads.emplace_back(std::bind(userConn, tmp, new_soc));
				else
				{
					p.op = Pack::OP_REPLY;
					p.reply = false;
					new_soc->sendobj(p);
				}
			}
			else if (p.op == Pack::OP_LOGIN)
			{
				//cout << "LOG" << endl;
				auto tmp = ServerDB::get_instance().login(p.namesp1.name, p.namesp1.passwd);
				if (tmp) threads.emplace_back(std::bind(userConn, tmp, new_soc));
				else
				{
					p.op = Pack::OP_REPLY;
					p.reply = false;
					new_soc->sendobj(p);
				}
			}
			else continue;
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
	assert(sizeof(Pack) == ServerSocket::MAX_SIZE);
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
