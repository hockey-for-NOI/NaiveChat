#pragma once

#include "ServerSocket.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace	NaiveChat
{

struct	User
{
	std::string	name;
	std::string	salted_passwd2;
	std::shared_ptr <ServerSocket>	status;

	static	std::string	salted(std::string const&);

	inline	User(std::string const&  name, std::string const& salted_passwd1):
		name(name), salted_passwd2(salted(salted_passwd1)), status(nullptr)
	{}

	inline	bool	login(std::string const& salted_passwd1) const
	{return salted_passwd2 == salted(salted_passwd1);}

	inline	void	cpwd(std::string const& salted_passwd1) {salted_passwd2 = salted(salted_passwd1);}
};

class	ServerDB
{
public:
	static	ServerDB&	get_instance();
	std::shared_ptr<User>	regist(std::string const& name, std::string const& sp1);
	std::shared_ptr<User>	login(std::string const& name, std::string const& sp1);
	std::vector <std::string>	listuser();
private:
	ServerDB();
	static	ServerDB*	m_instance;
	std::mutex	mtx;
	std::unordered_map <std::string, std::shared_ptr<User>> m_users;
};

}	// end namespace NaiveChat
