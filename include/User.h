#pragma once

#include "ServerSocket.h"
#include "Pack.h"

#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include <list>
#include <vector>
#include <string>

namespace	NaiveChat
{

class	User
{
private:
	std::string	name;
	std::string	salted_passwd2;
	std::shared_ptr <ServerSocket>	status;
	std::mutex mtx;

	std::unordered_map <std::string, std::shared_ptr<User>> friends;
	std::shared_ptr<User> chat_target;
	std::shared_ptr<std::thread> fwd;
	std::unordered_map <std::string, std::list<Pack> > chatbuf, filebuf;
	std::list <std::string> chatseq, fileseq;

	static	std::string	salted(std::string const&);

public:
	inline	User(std::string const&  name, std::string const& salted_passwd1):
		name(name), salted_passwd2(salted(salted_passwd1)), status(nullptr)
	{}

	std::string	getname();
	bool	login(std::string const& salted_passwd1);
	void	cpwd(std::string const& salted_passwd1);
	bool	isonline();
	void	setsoc(std::shared_ptr <ServerSocket>);
	void	closeconn();
	bool	add(std::string const& name, std::shared_ptr<User>);
	std::vector < std::pair <std::string, bool> >	listfriend();
	void	chat(std::shared_ptr<User> u = nullptr);
	void	recvchatpack(std::string const& src, Pack const& p);
	void	recvfilepack(std::string const& src, Pack const& p);
	void	forward(Pack const& p);
	bool	recvmsg(Pack &p);
	bool	recvfile(Pack &p);
};

}	// end namespace NaiveChat
