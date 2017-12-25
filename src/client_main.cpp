#include "ClientSocket.h"
#include "Pack.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <functional>

using	std::cin;
using	std::cout;
using	std::endl;
using	std::string;

using	NaiveChat::ClientSocket;
using	NaiveChat::Pack;

void	filerecv(std::shared_ptr<ClientSocket> soc, std::string filename)
{
	Pack p;
	std::string fullname = std::string(getenv("HOME")) + "/Downloads/" + filename;
	std::ofstream f(fullname, std::ios::binary);
	if (!f)
	{
		cout << "File " << fullname << " access failed. Trying to save in /tmp instead." << endl;
		f = std::ofstream("/tmp/" + filename, std::ios::binary);
	}
	do
	{
		cout << ">"; cout.flush();
		if (!soc->recvobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
		if (p.op != Pack::OP_FILEDATA) {cout << "File transmission interrupted." << endl; break;}
		short	size;
		memcpy(&size, p.filedata.len, sizeof(short));
		if (f) f.write(p.filedata.data, size);
	}	while (p.filedata.hasnext);
	cout << endl << "File transmission finished." << endl;
}

void	filesend(std::shared_ptr<ClientSocket> soc, std::string filename)
{
	Pack p;
	p.op = Pack::OP_FILEDATA;
	std::ifstream f(filename, std::ios::binary);
	if (!f) cout << "File not found." << endl;
	while(true)
	{
		cout << ">"; cout.flush();
		short size = 0;
		if (f)
		{
			f.read(p.filedata.data, 512);
			size = f.gcount();
		}
		memcpy(p.filedata.len, &size, sizeof(short));
		p.filedata.hasnext = (bool)f;
		if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
		if (!f) break;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	cout << endl << "File transmission finished." << endl;
}


void	chatterm(std::shared_ptr<ClientSocket> soc, std::string src, std::string dst, std::shared_ptr<std::mutex> mtx)
{
	Pack p;
	while (true)
	{
		mtx->lock(); mtx->unlock();
		cout << "[" << src << "](" << dst << ")>"; cout.flush();
		std::string s;
		cin >> s;
		if (s == "exit")
		{
			p.op = Pack::OP_STOPCHAT;
			if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			break;
		}
		if (s == "sendmsg")
		{
			std::getline(cin, s);
			if (s.length() > ClientSocket::MAX_SIZE - 2) {cout << "Message too long." << endl; continue;}
			p.op = Pack::OP_CHATMSG;
			strcpy(p.msg.name, src.c_str());
			strcpy(p.msg.data, s.c_str());
			if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			continue;
		}
		if (s == "sendfile")
		{
			cin >> s;
			string cores(std::find_if(s.rbegin(), s.rend(), [](char ch){return ch == '/';}).base(), s.end());
			if (cores.length() >= 500) {cout << "Filename too long." << endl; continue;}
			p.op = Pack::OP_FILEINFO;
			strcpy(p.fileinfo.username, src.c_str());
			strcpy(p.fileinfo.filename, cores.c_str());
			if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			filesend(soc, s);
			continue;
		}
		if (s == "recvfile")
		{
			p.op = Pack::OP_RECVFILE;
			if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}
		if (s != "help") cout << "Unrecognized command." << endl;
		cout << "help\tShow this message." << endl;
		cout << "sendmsg [MESSAGE]\tSend a message." << endl;
		cout << "sendfile [FILENAME]\tSend a file." << endl;
		cout << "recvfile [FILENAME]\tReceive the most recent file." << endl;
		cout << "exit\tQuit NaiveChat." << endl;
	}
}

void	chatrecv(std::shared_ptr<ClientSocket> soc, std::string src, std::shared_ptr<std::mutex> mtx)
{
	Pack p;
	while (true)
	{
		if (!soc->recvobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
		switch (p.op)
		{
			case Pack::OP_STOPCHAT:
				return;
			break;
			case Pack::OP_CHATMSG:
				cout << endl << p.msg.name << ": " << p.msg.data << endl;
			break;
			case Pack::OP_FILEINFOC:
				cout << endl << "File from " << p.fileinfo.username << ": " << p.fileinfo.filename << endl;
			break;
			case Pack::OP_REPLY:
				cout << endl << "No files available." << endl;
			break;
			case Pack::OP_FILEINFO:
				mtx->lock();
				if (p.op == Pack::OP_REPLY)
					cout << endl << "No new files." << endl;
				else
				{
					cout << endl << "File from " << p.fileinfo.username << ": " << p.fileinfo.filename << endl;
					filerecv(soc, p.fileinfo.filename);
				}
				mtx->unlock();
			break;
		}
	}
}

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
		string tmp;
		std::getline(cin, tmp);
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
			cout << "Username: "; cout.flush();
			std::getline(cin, name);
			if (name.length() < 1) {cout << "Invalid username length (<1)." << endl; break;}
			if (name.length() > 20) {cout << "Invalid username length (>20)." << endl; break;}
			cout << "Password: "; cout.flush();
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
		cin >> s;
		if (s == "exit") break;
		if (s == "cpwd")
		{
			string passwd;
			cin >> passwd;
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
		if (s == "add")
		{
			string name;
			cin >> name;
			if (name.length() < 1) {cout << "Invalid username length (<1)." << endl; break;}
			if (name.length() > 20) {cout << "Invalid username length (>20)." << endl; break;}
			Pack p;
			p.op = Pack::OP_ADD;
			strcpy(p.addfriend.name, name.c_str());
			if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			if (!soc->recvobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			if (p.op != Pack::OP_REPLY or !p.reply) {cout << "Friend not found." << endl; continue;}
			cout << "Success." << endl;
			continue;
		}
		if (s == "ls")
		{
			Pack p;
			p.op = Pack::OP_LISTFRIEND;
			if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			bool flag = false;
			do
			{
				if (!soc->recvobj(p)) {flag = true; break;}
				auto &tar = p.listfriendres;
				for (char i=0; i<tar.len && i < 20; i++)
					cout << tar.name[i] << "(" << (tar.online[i] ? "Online" : "Offline") << ")" << endl;
			}	while (p.listfriendres.hasnext);
			if (flag) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			continue;
		}
		if (s == "chat")
		{
			auto mtx = std::make_shared<std::mutex>();
			string name;
			cin >> name;
			Pack p;
			p.op = Pack::OP_STARTCHAT;
			strcpy(p.chat.name, name.c_str());
			if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			if (!soc->recvobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			if (p.op != Pack::OP_REPLY || !p.reply) {cout << "Chat denied by server." << endl; continue;}
			std::thread t1(std::bind(chatterm, soc, username, name, mtx));
			std::thread t2(std::bind(chatrecv, soc, name, mtx));
			t1.join();
			t2.join();
			continue;
		}
		if (s == "recvmsg")
		{
			Pack p;
			p.op = Pack::OP_RECVMSG;
			if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			if (!soc->recvobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			if (p.op == Pack::OP_CHATMSG)
				cout << endl << p.msg.name << ": " << p.msg.data << endl;
			else cout << "No new messages." << endl;
			continue;
		}
		if (s == "recvfile")
		{
			Pack p;
			p.op = Pack::OP_RECVFILE;
			if (!soc->sendobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			if (!soc->recvobj(p)) {cout << "Connection unexpectly closed by remote host." << endl; break;}
			if (p.op == Pack::OP_REPLY)
				cout << endl << "No new files." << endl;
			else
			{
				cout << endl << "File from " << p.fileinfo.username << ": " << p.fileinfo.filename << endl;
				filerecv(soc, p.fileinfo.filename);
			}
			continue;
		}
		if (s != "help") cout << "Unrecognized Command." << endl;
		cout << "help\tShow this message." << endl;
		cout << "cpwd [newpass]\tChange password." << endl;
		cout << "search\tList online user." << endl;
		cout << "add [userid]\tAdd user as friend." << endl;
		cout << "ls\tList friends status." << endl;
		cout << "chat [userid]\tChat with user." << endl;
		cout << "recvmsg\tReceive most recent message." << endl;
		cout << "recvfile\tReceive most recent file." << endl;
		cout << "exit\tQuit NaiveChat." << endl;
	}
	return 0;
}
