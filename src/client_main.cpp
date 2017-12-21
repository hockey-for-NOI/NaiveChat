#include "ClientSocket.h"
#include "Pack.h"
#include <cstring>
#include <iostream>
#include <string>
#include <memory>

using	std::cin;
using	std::cout;
using	std::endl;
using	std::string;

using	NaiveChat::ClientSocket;
using	NaiveChat::Pack;

int	main()
{
	auto soc = std::make_shared<ClientSocket>();
	string	username;
	while (!cin.eof())
	{
		soc = std::make_shared<ClientSocket>();
		cout << "Welcome to NaiveChat Client!" << endl;
		cout << "Server IP: "; cout.flush();
		string host;
		std::getline(cin, host);
		cout << "Server Port: "; cout.flush();
		int port;
		cin >> port;
		std::getline(cin, host);
		cout << "Connecting..."; cout.flush();
		if (!soc->conn(host.c_str(), port))
		{
			cout << "Failed." << endl;
			continue;
		}	else	cout << "Done." << endl;
		bool	flag = false;
		while (true)
		{
			cout << "Register(R)/Login(l)?";
			string op;
			std::getline(cin, op);
			Pack p;
			if (op == "" or op == "R" or op == "r") p.op = Pack::OP_REGIST;
			else if (op == "L" or op == "l") p.op = Pack::OP_LOGIN;
			else continue;
			string name, passwd;
			cout << "Username: ";
			std::getline(cin, name);
			if (name.length() < 1) {cout << "Invalid username length (<1)." << endl; break;}
			if (name.length() > 20) {cout << "Invalid username length (>20)." << endl; break;}
			cout << "Password: ";
			std::getline(cin, passwd);
			if (passwd.length() < 2) {cout << "Invalid password length (<2)." << endl; break;}
			if (passwd.length() > 20) {cout << "Invalid password length (>20)." << endl; break;}
			strcpy(p.namesp1.name, name.c_str());
			strcpy(p.namesp1.passwd, passwd.c_str());
			if (!soc->sendobj(p)) {cout << "Connection Failed." << endl; break;}
			if (!soc->recvobj(p)) {cout << "Connection Failed." << endl; break;}
			if (p.op != Pack::OP_REPLY) {cout << "Unknown reply type." << endl; break;}
			if (!p.reply) {cout << "Authentification Failed." << endl; break;}
			cout << "Success!" << endl;
			flag = 1; username = name; break;
		}
		if (flag) break;
	}
	while (!cin.eof())
	{
		cout << "[" << username << "]>"; cout.flush();
		string s;
		std::getline(cin, s);
		if (s == "exit") break;
		if (s == "cpwd")
		{
			string passwd;
			cout << "Password: ";
			std::getline(cin, passwd);
			if (passwd.length() < 2) {cout << "Invalid password length (<2)." << endl; break;}
			if (passwd.length() > 20) {cout << "Invalid password length (>20)." << endl; break;}
			Pack p;
			p.op = Pack::OP_CPWD;
			p.cpwd[passwd.length()] = 0;
			memcpy(p.cpwd, passwd.c_str(), passwd.length());
			if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			continue;
		}
		if (s == "search") 
		{
			Pack p;
			p.op = Pack::OP_SEARCH;
			if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			bool flag = false;
			do
			{
				if (!soc->recvobj(p)) {flag = true; break;}
				for (char i=0; i<p.searchres.len && i < 20; i++)
					cout << p.searchres.name[i] << endl;
			}	while (p.searchres.hasnext);
			if (flag) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			continue;
		}
		if (s != "help") cout << "Unrecognized Command." << endl;
		cout << "help\tShow this message." << endl;
		cout << "cpwd\tChange password." << endl;
		cout << "exit\tQuit NaiveChat." << endl;
	}
	return 0;
}
