#pragma once

#include "ServerSocket.h"
#include "Pack.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

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
	std::vector <Pack> buffer;

	static	std::string	salted(std::string const&);

public:
	inline	User(std::string const&  name, std::string const& salted_passwd1):
		name(name), salted_passwd2(salted(salted_passwd1)), status(nullptr)
	{}

	bool	login(std::string const& salted_passwd1);
	void	cpwd(std::string const& salted_passwd1);
	bool	isonline();
	void	setsoc(std::shared_ptr <ServerSocket>);
	void	closeconn();
	bool	add(std::string const& name, std::shared_ptr<User>);
	std::vector < std::pair <std::string, bool> >	listfriend();
};

class	ServerDB
{
public:
	static	ServerDB&	get_instance();
	std::shared_ptr<User>	regist(std::string const& name, std::string const& sp1);
	std::shared_ptr<User>	login(std::string const& name, std::string const& sp1);
	std::vector < std::pair <std::string, bool> >	listuser();
	std::shared_ptr<User>	getuser(std::string name);
private:
	ServerDB();
	static	ServerDB*	m_instance;
	std::mutex	mtx;
	std::unordered_map <std::string, std::shared_ptr<User>> m_users;
};

}	// end namespace NaiveChat
